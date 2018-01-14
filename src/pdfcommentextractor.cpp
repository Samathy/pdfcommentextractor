#include <iostream>
#include <string>
#include <stdlib.h> //realpath
#include <stdio.h>
#include <string.h>
#include <vector>

//TODO reorder these includes

#include <glib.h>
#include <poppler.h>




class document
{

    public:
        document(std::string filename)
        {
            std::cout << "Creating document" << std::endl;
            GError * error;
            //glib requires URIs (file:///home/foo/bar/baz.pdf)
            gchar * filename_uri = g_filename_to_uri(filename.c_str(), NULL , &error);
            if (error != NULL)
            {   
                std::cout << "Error getting URI" << std::endl;
                printf(error->message);
                return; 
            }

            g_clear_error(&error);

            this->popp_doc = poppler_document_new_from_file(filename_uri, NULL, &error);

            if (error != NULL)
            {
                std::cout << "Error Opening file" << std::endl;
                printf(error->message);
                return;
            }
            this->numPages = poppler_document_get_n_pages(this->popp_doc);
        }

        const int getNumPages()
        {
            return this->numPages;
        }

        const PopplerPage * getPage(int index)
        {
            std::cout << "Getting Page" << index << std::endl;

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
        std::vector<std::string> getComments(const PopplerPage * page)
        {
            std::cout << "Getting comments" << std::endl;

            std::vector<std::string> comments; 
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

                    comments.push_back(std::string(poppler_page_get_text_for_area((PopplerPage*)page, &rect)));
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



int main(int argv, char ** argc)
{
    //TODO add getopt arguments.

    GError * error = nullptr;

    //TODO Add checks to make sure we have the right number of args
    char * filename = realpath(argc[1], NULL);

    document * d = new document(std::string(filename));

    std::vector<std::string> comments;

    int numPages = d->getNumPages();
    int i = 0;
    while (i < numPages)
    {
        std::vector<std::string> tmpComments;
        tmpComments = d->getComments(d->getPage(i));
        std::copy(tmpComments.begin(), tmpComments.end(), std::back_inserter(comments));
        i++;
    }

    for (std::string s: comments)
    {
        std::cout << "-----------------------------" << std::endl;
        std::cout << s << std::endl;
    }

    return 0;

}
