#include <iostream>
#include <string>
#include <stdlib.h> //realpath
#include <string.h>

#include <glib.h>
#include <poppler.h>



int main(int argv, char ** argc)
{

    GError * error = nullptr;

    //TODO Add checks to make sure we have the right number of args
    char * filename = realpath(argc[1], NULL);

    //glib requires URIs (file:///home/foo/bar/baz.pdf)
    gchar * filename_uri = g_filename_to_uri(filename,NULL , &error);
    if (error != NULL)
    {
        std::cout << error->message << std::endl;
        return 0;
    }

    free(filename);
    g_clear_error(&error);

    PopplerDocument * doc = poppler_document_new_from_file(filename_uri, NULL, &error);

    if (error != NULL)
    {
        std::cout << error->message << std::endl;
        return 0;
    }


    const int numPages = poppler_document_get_n_pages(doc);

    std::cout << "Number of pages in the document: " << numPages << std::endl;

    for (int i=0;i<numPages;i++)
    {
        PopplerPage * page = poppler_document_get_page(doc, i);

        GList * annotations = poppler_page_get_annot_mapping(page);

        for ( GList * l = annotations; l != NULL; l = l->next)
        {
            PopplerAnnotMapping * annotation = (PopplerAnnotMapping*)l->data;
            PopplerAnnotType type = poppler_annot_get_annot_type(annotation->annot);
            if (type == POPPLER_ANNOT_HIGHLIGHT)
            {
                PopplerRectangle rect;
                poppler_annot_get_rectangle(annotation->annot, &rect);
                char * text = poppler_page_get_text_for_area(page, &rect);
                std::cout << text;
                return 0 ;
            }

        }

        poppler_free_page_annot_mapping(page);

    }


    return 0;

}
