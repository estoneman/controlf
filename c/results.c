#include <errno.h>                    // error checking for libcurl
#include <curl/curl.h>  
#include "urldecoder.c"

#define BODY "<body>"
#define END_BODY "</body>"

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

// convert ASCII to UTF-8
void parseForm(char* str) {

    int i = 0, j, n = strlen(str);

    // find index at which the first equals sign is
    while (str[i++] != '=');

    // starting at index (index of '='), reassign the rest of the characters
    for (j = 0; i < n; i++)
        str[j++] = str[i];

    str[j] = '\0';

}

char* body(char *html) {
    char *begin = strstr(html, BODY) + 6;
    char *end = strstr(html, END_BODY);
    size_t len = end - begin;
    char *result = (char*)malloc(sizeof(char)*(len + 1));
    strncpy(result, begin, len);
    result[len] = '\0';
    return result;
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
    printf("<title>Results\n");
    printf("</title>\n");
    printf("</head>\n");
    printf("<body>\n");

    // content length is the amount of bytes that are being read
    contentLenString = getenv("CONTENT_LENGTH");
    contentLength = atoi(contentLenString);

    if ( contentLength ) {

        // Allocate and set memory to read stdin data into (cgi form input)
        stdInData = malloc(contentLength);
        if ( stdInData )
            memset(stdInData, 0x00, contentLength);
        else
            printf("ERROR: Unable to allocate memory\n");

        // this calculates the amount of bytes that was successfully read into memory
        bytesRead = fread((char*)stdInData, 1, contentLength, stdin);

        // If we successfully read all bytes from stdin, format and
        // write the data to stdout using the writeData function.

        parseForm(stdInData);

        // if these do not match then there was an error reading the data
        if ( bytesRead != contentLength )
            printf("<br>Error reading standard input\n");

        // decode ASCII url to UTF-8 to be able to search a url e.g. https%3A%2F%2Fgoogle.com --> https://google.com
        char* url = malloc(strlen(stdInData) + 1);
        urldecode2(url, stdInData);

        // Free the storage allocated to hold the stdin data
        free(stdInData);

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

        // printf("<br>%lu bytes retrieved<br>\n", (unsigned long)chunk.size);

        chunk.response = body(chunk.response);
        while (*chunk.response) printf("%c\n", *chunk.response++);

        // free any memory used to store input data
        free(chunk.response);
	    free(url);

        // cleanup any memory leaks
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();

    }   
    else
        printf("<br><br><b>There is no standard input data.</b>");

    // Write the closing tags on HTML document
    printf("</body>\n");
    printf("</html>\n");

    return 0;
}
