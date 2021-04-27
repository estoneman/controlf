#include <ctype.h>
#include <curl/curl.h>
#include <errno.h>                    // error checking header
#include <stdlib.h>
#include <string.h>

#include "../include/algos.h"

typedef struct request {
    char* response;
    size_t size;
} HT_Request;

// add rest of prototypes when project is nearing its end
static size_t write_data(void *data, size_t size, size_t nmemb, void *stream);

// write callback function
static size_t write_data(void *data, size_t size, size_t nmemb, void *stream) {
	size_t realsize = size*nmemb;
	HT_Request *mem = (HT_Request *)stream;

	char *ptr = realloc(mem->response, mem->size + realsize + 1);
	if (ptr == NULL)
		return 0; // out of memory error

	mem->response = ptr;
	memcpy(&(mem->response[mem->size]), data, realsize);
	mem->size += realsize;
	mem->response[mem->size] = 0;

	return realsize;
}

int main()
{

    char    *contentLenString;            /* content length as a string which is implicitly typed from getenv()  */
    char    *decoded_search_string;       /* decoded search string (ascii to utf8)                               */
    char    *query_string;                /* memory location for the entire query string using POST              */

    size_t  bytesRead;                    /* number of bytes read into memory                                    */
    size_t  contentLength;                /* number of characters that was passed from the form                  */

    // The "Content-type" is the minimum request header that must be written to standard output.  It describes the type of data that follows.                                                         */
    printf("Content-type: text/html\r\n\r\n");
    printf("<html>\n");
    printf("<head>\n");
    printf("<title>Results</title>\n");
    printf("<link rel = 'shortcut icon' type = 'image/jpg' href = '../img/conky.jpeg'>");
    // printf("<link rel = 'stylesheet' href = 'http://controlf.com/index.css'>");
    printf("</head>\n");
    printf("<body>\n");

    // content length is the amount of bytes that are being read
    contentLenString = getenv("CONTENT_LENGTH");
    contentLength = (size_t) atoi(contentLenString);

    if ( contentLength ) {

        // Allocate and set memory to read stdin data into (cgi output using POST method)
        // calloc usually handles its own out-of-bounds errors
        query_string = (char *)calloc(1, contentLength + 1);

        // this calculates the amount of bytes that was successfully read into memory from the query string
        bytesRead = fread(query_string, 1, contentLength, stdin);

        char *ascii_url = (char *)calloc(1, contentLength + 1);
        size_t ascii_url_len = strlen(ascii_url);
        char *search_string = (char *)calloc(1, contentLength + 1);

        // put url and search string into their own places in memory
        parseForm(query_string, contentLength, ascii_url, search_string);

        // if these do not match then there was an error reading the data
        if ( bytesRead != contentLength )
            printf("<br>Error reading standard input\n");

        CURL *curl_handle;
        long info;

        HT_Request request;
        CURLcode res;
        request.response = malloc(1);
        request.size = 0;

        curl_global_init(CURL_GLOBAL_ALL);

        // initialize curl (Client URL) session
        curl_handle = curl_easy_init();

        // decode ASCII url to UTF-8 to be able to search a url e.g. https%3A%2F%2Fgoogle.com --> https://google.com
        char *escaped_url = curl_easy_unescape(curl_handle, ascii_url, ascii_url_len, NULL);

        // setting browser to mozilla because certain resources were forbidden by certain websites (for example, hulu is weird about this)
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/4.0");

        // set the URL to retrieve here
        curl_easy_setopt(curl_handle, CURLOPT_URL, escaped_url);

        // switch on full protocol/debug output while testing
        curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

        // disable progress meter, set to 0L to enable it
        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

        // send all data to this function
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

        // pass our request data to function to be written into
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&request);

        // check if operation was successful
        if ( (res = curl_easy_perform(curl_handle) ) != CURLE_OK)
            // fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            printf("%s\n", curl_easy_strerror(res));

        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &info);

        printf("<br>Response Code: %ld<br><br>", info);

        printf("<br>%zu bytes fetched from %s<br><br>\n", request.size, escaped_url);

        plain_text(request.response);

        // convert ascii search string to utf-8 search string
        decoded_search_string = calloc(1, strlen(search_string) + 1);
        utf8_decode(decoded_search_string, search_string);

        // single iterative find algorithm
        size_t numfound = s_iter_search(request.response, search_string);

        printf("<h3>%zu Occurence(s) of %s\n</h3><br>", numfound, decoded_search_string);

        // // finally write contents of request.response for debug purposes
        // FILE *fp;
        // fp = fopen("../scrapedhtml/scraped.txt", "w");
        // while (!isspace(*request.response++)) fprintf(fp, "%c", *request.response);
        // // fprintf(fp, "%s\n", request.response);
        // fclose(fp);

        // free any memory
        free(decoded_search_string);
        free(request.response);
        free(ascii_url);
        free(query_string);

        // cleanup any memory leaks
        curl_free(escaped_url);
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();

    }
    else
        printf("<br><br><b>There is no standard input data.</b>");

    // closing tag of html
    printf("</body>\n");
    printf("</html>\n");

    return 0;
}
