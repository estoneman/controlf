#include <errno.h>                    // error checking for libcurl
#include <curl/curl.h>  
#include "urldecoder.c"

#define BODY "<body>"
#define END_BODY "</body>"

static size_t write_data(void *data, size_t size, size_t nmemb, void *stream);
void parseForm(char *query_string, size_t query_string_len, char *url, char *search_string);
void body(char *raw_html);

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

// write each form value into their own separate variables
void parseForm(char *query_string, size_t query_string_len, char *url, char *search_string) {

    int eq_i1 = 0, eq_i2 = 0, amp_i = 0;

    while (query_string[eq_i1++] != '=');
    amp_i = eq_i1;
    while (query_string[++amp_i] != '&');
    eq_i2 = amp_i;
    while (query_string[eq_i2++] != '=');

    for (; eq_i1 < amp_i; eq_i1++) strncat(url, &query_string[eq_i1], 1);
    for (; eq_i2 < query_string_len; eq_i2++) strncat(search_string, &query_string[eq_i2], 1);

}

// char* body(char *html) {
//     char *begin = strstr(html, BODY) + 6;
//     char *end = strstr(html, END_BODY);
//     size_t len = end - begin;
//     char *result = (char*)malloc(sizeof(char)*(len + 1));
//     strncpy(result, begin, len);
//     result[len] = '\0';
//     return result;
// }

void body(char *raw_html) {
    char *plain_text = raw_html;
    while(*raw_html) {
        if(*plain_text == '<')
            while(*plain_text && *plain_text++ != '>');
        *raw_html++ = *plain_text++; 
    }
}

int main()
{

    char   *query_string;                /* Input buffer.               */
    char   *contentLenString;            /* Character content length.   */
    int    contentLength;                /* int content length          */
    int    bytesRead;                    /* number of bytes read.       */

    // The "Content-type" is the minimum request header that must be written to standard output.  It describes the type of data that follows.                                                         */
    printf("Content-type: text/html\r\n\r\n");
    printf("<html>\n");
    printf("<head>\n");
    printf("<title>Results</title>\n");
    printf("</head>\n");
    printf("<body>\n");

    // content length is the amount of bytes that are being read
    contentLenString = getenv("CONTENT_LENGTH");
    contentLength = atoi(contentLenString);

    if ( contentLength ) {

        // Allocate and set memory to read stdin data into (cgi form input)
        query_string = malloc(contentLength);
        if ( query_string )
            memset(query_string, 0x00, contentLength);
        else
            printf("ERROR: Unable to allocate memory\n");

        // this calculates the amount of bytes that was successfully read into memory from the query string
        bytesRead = fread((char*)query_string, 1, contentLength, stdin);

        // If we successfully read all bytes from stdin, format and
        // write the data to stdout using the writeData function.
        char *ascii_url = malloc(contentLength);
        char *search_string = malloc(contentLength);

        // put url and search string into their own places in memory
        parseForm(query_string, contentLength, ascii_url, search_string);

        // if these do not match then there was an error reading the data
        if ( bytesRead != contentLength )
            printf("<br>Error reading standard input\n");

        // decode ASCII url to UTF-8 to be able to search a url e.g. https%3A%2F%2Fgoogle.com --> https://google.com
        char *utf_url = malloc(contentLength + 1);
        urldecode2(utf_url, ascii_url);

        CURL *curl_handle;

        struct memory chunk;
        CURLcode res;
        chunk.response = malloc(1);
        chunk.size = 0;

        curl_global_init(CURL_GLOBAL_ALL);

        // initialize curl (Client URL) session
        curl_handle = curl_easy_init();

        // set the URL to retrieve here
        curl_easy_setopt(curl_handle, CURLOPT_URL, utf_url);

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

        printf("%zu bytes fetched from %s<br>\n", chunk.size, utf_url);

        body(chunk.response);
        printf("%s", chunk.response);

        // free any memory used to store input data
        free(chunk.response);
        free(ascii_url);
	    free(utf_url);
        free(query_string);

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
