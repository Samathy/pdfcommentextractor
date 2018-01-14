#include <iostream>
#include <string>
#include <stdlib.h> //realpath
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <vector>
#include <exception>
#include <tuple>
#include <unistd.h>
#include <fstream>

//TODO reorder these includes

#include <glib.h>
#include <poppler.h>

class exceptionWithMessage: public std::exception
{
    public:
    std::string message;
    exceptionWithMessage(std::string s) : message(s){};
    ~exceptionWithMessage() throw () {};
    const char * what() const throw() {return message.c_str();};

};


class document
{

    public:
        document(std::string filename)
        {
            GError * error = nullptr;

            //glib requires URIs (file:///home/foo/bar/baz.pdf)
            gchar * filename_uri = g_filename_to_uri(filename.c_str(), NULL , &error);

            if (error != NULL)
            {   
                std::cout << "Error getting URI for filename:"<< filename << std::endl;
                throw exceptionWithMessage(error->message);
            }

            g_clear_error(&error);

            this->popp_doc = poppler_document_new_from_file(filename_uri, NULL, &error);

            if (error != NULL)
            {
                std::cout << "Error Opening file" << std::endl;
                throw error->message;
            }
            this->numPages = poppler_document_get_n_pages(this->popp_doc);
        }

        const int getNumPages()
        {
            return this->numPages;
        }

        const PopplerPage * getPage(int index)
        {
            if (index > this->numPages)
            {
                //We could throw here, or we could be lazy.
                return NULL;
            }

            return poppler_document_get_page(this->popp_doc, index);
        }
        
        /** \brief Gets all the annotations for a given page  */
        GList * getAnnotationsList(const PopplerPage * page)
        {
            return poppler_page_get_annot_mapping((PopplerPage*)page);
        }

        /** Returns a vector of comments for a given page */
        std::vector<std::tuple<int, std::string>> getComments(const PopplerPage * page)
        {
            std::vector<std::tuple<int,std::string>> comments; 
            //TODO make it a vector of pairs and return the page number and title and stuff.
            GList * annotList = getAnnotationsList(page);
            for (GList * l = annotList; l !=NULL; l = l->next)
            {
                PopplerAnnotMapping * annotMapping = (PopplerAnnotMapping*)l->data;

                PopplerAnnotType type = poppler_annot_get_annot_type(annotMapping->annot);
                if (poppler_annot_get_annot_type(annotMapping->annot) == POPPLER_ANNOT_HIGHLIGHT)
                {
                    PopplerRectangle rect;

                    poppler_annot_get_rectangle(annotMapping->annot, &rect);
                    std::tuple<int, std::string>comment (poppler_page_get_index((PopplerPage*)page), std::string(poppler_page_get_text_for_area((PopplerPage*)page, &rect)));
                    comments.push_back(comment);
                }
            }
            poppler_page_free_annot_mapping(annotList);

            return comments;
        }


    private:
        std::string filename;

        PopplerDocument * popp_doc;
        int numPages;


};

/** \brief Unwraps the text to a given line width */
std::string textUnwrap(std::string text, int linewidth)
{

    std::string text_copy = text;
    int lastFoundLineEnding;
    size_t firstLineEnding;

    while((firstLineEnding = text_copy.find("-\n", 0)) != std::string::npos)
    {    text_copy = text_copy.replace(firstLineEnding, 2, "");    }

    while((firstLineEnding = text_copy.find("\n", 0)) != std::string::npos)
    {    text_copy = text_copy.replace(firstLineEnding, 1, " ");    }
    
    for (int i = 0; i<text_copy.length(); i=i+linewidth)
    {
        if(i == 0)
        {    continue;    }
        int loopback = i;
        while(text_copy[i] != ' ')
        {    i--;    }
        text_copy.replace(i, 1, "\n");
    }

    return text_copy;

    //Unwrap text and remove hyphens
    //Wrap to given linewidth
    
}



int main(int argc, char ** argv)
{

    bool pageNums = false;
    bool unwrap = false;
    int unwrapWidth = 80;
    bool allPages = true;
    bool tostdout = true;
    std::string filename;
    std::string outputFilename;
    std::vector<int> pages;

    char c;

    std::ostream * output = &std::cout;

    while ((c = getopt(argc, argv, "pu:P:f:o:h")) != -1)
    {
        switch(c)
        {
            case 'p':
                pageNums = true;
                break;
            case 'u':
                unwrap = true;
                unwrapWidth = std::stoi(optarg);
                break;
            case 'P':
                allPages = false;
                pages.push_back(std::stoi(optarg));
                break;
            case 'f':
                filename = std::string(realpath(optarg, NULL));
                break;
            case 'o':
                tostdout = false;
                outputFilename = std::string(optarg);
                break;
            case 'h':
                std::cout << "Usage: pdfcommentextractor [-pu] -f [filename] -P [specificPage] -o [ouputFile]" << std::endl;
                break;
            case '?':
                std::cout << "Usage: pdfcommentextractor [-pu] -f [filename] -P [specificPage] -o [ouputFile]" << std::endl;
                break;
        }

    }

    //TODO add getopt arguments.
    //add page numbers, y/n
    //only select given page 
    //unwrap text

    GError * error = nullptr;

    //TODO Add checks to make sure we have the right number of args

    document * d = new document(filename);

    std::vector<std::tuple<int, std::string>> comments;

    int numPages = d->getNumPages();
    if (!allPages)
    {
        for(int pageNumber: pages)
        {
            std::cout << pageNumber << std::endl;
            if(pageNumber < numPages)
            {
                std::vector<std::tuple<int, std::string>> tmpComments;
                tmpComments = d->getComments(d->getPage(pageNumber));
                std::copy(tmpComments.begin(), tmpComments.end(), std::back_inserter(comments));
            }
        }
    }
    else
    {
        int i = 0;
        while (i < numPages)
        {
            std::vector<std::tuple<int, std::string>> tmpComments;
            tmpComments = d->getComments(d->getPage(i));
            std::copy(tmpComments.begin(), tmpComments.end(), std::back_inserter(comments));
            i++;
        }
   }

    if (!tostdout && !outputFilename.empty())
    {
        output = new std::ofstream(outputFilename, std::ofstream::out|std::ofstream::app);
    }

    for (std::tuple<int, std::string> s: comments)
    {
        *output << std::string(unwrapWidth, '-') << std::endl;
        if (pageNums)
        {    *output << "Page: " << std::get<0>(s) << std::endl;    }
        if (unwrap)
        {    *output << textUnwrap(std::get<1>(s), unwrapWidth) << std::endl;    }
        else
        {    *output << std::get<1>(s) << std::endl;    }
    }
    if(!tostdout)
    {    delete(output);    }

    delete(d);

    return 0;

}
