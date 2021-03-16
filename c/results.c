#include <errno.h>                    // errno values
#include <curl/curl.h>  
#include "urldecoder.c"

struct memory {
    char* response;
    size_t size;
};

// write callback function
static size_t write_data(void *data, size_t size, size_t nmemb, void *stream) {
	size_t realsize = size*nmemb;
	struct memory *mem = (struct memory *)stream;

	char *ptr = realloc(mem->response, mem->size + realsize + 1);
	if (ptr == NULL)
		return 0; // out of memory error
	
	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;

	return realsize;
}

void parseURL(char* str) {

    int i = 0, j, n = strlen(str);

    // find index at which the first equals sign is
    while (str[i++] != '=');

    // starting at index (index of '='), reassign the rest of the characters
    for (j = 0; i < n; i++)
        str[j++] = str[i];

    str[j] = '\0';

}

int main()
{

    char   *stdInData;                   /* Input buffer.               */
    char   *contentLenString;            /* Character content length.   */
    int    contentLength;                /* int content length          */
    int    bytesRead;                    /* number of bytes read.       */

    // The "Content-type" is the minimum request header that must be written to standard output.  It describes the type of data that follows.                                                         */
    printf("Content-type: text/html\r\n\r\n");
    printf("<html>\n");
    printf("<head>\n");
    printf("<title>\n");
    printf("Results\n");
    printf("</title>\n");
    printf("</head>\n");
    printf("<body>\n");

    /*--------------------------------------------------------------*/
    /* The REQUEST_METHOD is "POST".  The environment variable      */
    /* CONTENT_LENGTH will tell us how many bytes of data to read   */
    /* from stdin.  Note: CONTENT_LENGTH must be convert to an int. */
    /*--------------------------------------------------------------*/
    contentLenString = getenv("CONTENT_LENGTH");
    contentLength = atoi(contentLenString);

    if ( contentLength ) {

        printf("<h4>Content Length<h4>%i\n", contentLength);

        // Allocate and set memory to read stdin data into
        stdInData = malloc(contentLength);
        if ( stdInData )
            memset(stdInData, 0x00, contentLength);
        else
            printf("ERROR: Unable to allocate memory\n");
        /*----------------------------------------------------------*/
        /* A CGI program MUST read standard input as a stream       */
        /* file only up to and including CONTENT_LENGTH bytes.      */
        /* Never should a program read more than CONTENT_LENGTH     */
        /* bytes.  A CGI program that reads standard input must     */
        /* never depend on an end of file flag.  This will cause    */
        /* unpredictable results when the CGI program reads         */
        /* standard input.                                          */
        /*----------------------------------------------------------*/
        printf("<h4>URL:</h4>\n");
        bytesRead = fread((char*)stdInData, 1, contentLength, stdin);

        // If we successfully read all bytes from stdin, format and
        // write the data to stdout using the writeData function.

        parseURL(stdInData);

        if ( bytesRead != contentLength )
            printf("<br>Error reading standard input\n");

        char* url = malloc(strlen(stdInData) + 1);
        urldecode2(url, stdInData);

        // Free the storage allocated to hold the stdin data

        CURL *curl_handle;

        struct memory chunk;
        CURLcode res;
        chunk.response = malloc(1);
        chunk.size = 0;

        curl_global_init(CURL_GLOBAL_ALL);

        // initialize curl (Client URL) session
        curl_handle = curl_easy_init();

        // set the URL to retrieve here
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);

        // switch on full protocol/debug output while testing
        curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

        // disable progress meter, set to 0L to enable it
        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

        // send all data to this function
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

        // pass our chunk of memory struct to function to be written into
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

        // check if operation was successful
        if ( !(res = curl_easy_perform(curl_handle)) )
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        printf("<br>%lu bytes retrieved\n", (unsigned long)chunk.size);

        unsigned int i;
        for (i = 0; i < (unsigned int)chunk.size; i++)
            printf("%c", *(chunk.response + i));

        // cleanup any memory leaks
        curl_easy_cleanup(curl_handle);

        // free any memory used to store input data
        free(chunk.response);

        curl_global_cleanup();

        free(stdInData);

    }   
    else
        printf("<br><br><b>There is no standard input data.</b>");

    // Write the closing tags on HTML document
    printf("</body>\n");
    printf("</html>\n");

    return 0;
}
