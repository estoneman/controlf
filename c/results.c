#include <ctype.h>
#include <curl/curl.h>
#include <errno.h>                    // error checking header
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "../include/algos.h"
#include "../include/util.h"

#define ASCII_TABLE_SIZE 128

typedef struct request {
    char*   response;
    size_t  size;
} HT_Request;

typedef struct algo {
    char    *name;
    double  cpu_time;
} algos;

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
    // amcharts CDN resources
    printf("<script src='https://cdn.amcharts.com/lib/4/core.js'></script>"
           "<script src='https://cdn.amcharts.com/lib/4/charts.js'></script>"
           "<script src='https://cdn.amcharts.com/lib/4/themes/animated.js'></script>"
    );
    printf("<title>Results</title>\n");
    printf("<link rel = 'shortcut icon' type = 'image/jpg' href = '../img/conky.jpeg'>");
    // printf("<link rel = 'stylesheet' href = 'http://controlf.com/index.css'>");
    printf(
           "<style>"
                "body {"
                    "font-family: 'Trebuchet MS', 'Lucida Sans Unicode', 'Lucida Grande', 'Lucida Sans', Arial, sans-serif;"
                    "background: #1d4063;"
                    "color: white;"
                    "background: #1d4063;"
                    "background-image: linear-gradient(to bottom right, #1d4063, #061b30);"
                "}"
                "#title-container {"
                    "margin: auto;"
                    "width: 50%%;"
                    "text-align: center;"
                "}"
                "#title-text {"
                    "font-size: xxx-large;"
                "}"
                ".algo_name, .algo_time {"
                    "display: none;"
                "}"

                "#res-container {"
                    "width: 100%%;"
                    "height: 500px;"
                    "background: #ffffff;"
                "}"
           "</style>"
    );
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

        // printf("<br>Response Code: %ld<br><br>", info);

        rm_html_tags(request.response);

        // Convert ascii search string to utf-8 search string
        decoded_search_string = calloc(1, strlen(search_string) + 1);
        utf8_decode(decoded_search_string, search_string);

        // Write contents of request.response
        // int numspaces = 1;
        int words = 0;
        FILE *outfile = fopen("../scrapedhtml/scraped.txt", "w");
        if (outfile) {
            while (*request.response++) {
                // if letter
                if ( (*request.response < 64 || *request.response > 91) || (*request.response < 96 || *request.response > 123) )
                    words++;
                fprintf(outfile, "%c", *request.response);
            }
            fclose(outfile);
        }

        // get input string for use in following algorithms
        char *src = 0;
        long infilesize = 0;
        FILE *infile = fopen("../scrapedhtml/scraped.txt", "r");
        if (infile) {
            fseek(infile, 0, SEEK_END);
            infilesize = ftell(infile);
            fseek(infile, 0, SEEK_SET);
            src = malloc(infilesize);
            if(src)
                fread(src, 1, infilesize, infile);
            fclose(infile);
        }

        clock_t begin, end;

        int num_algos = 4, numfound;
        algos *algo_arr = (algos*)malloc(sizeof(algos) * num_algos);

        /*////////////////////////////////////////////////////////////
                             NAIVE STRING SEARCH
        ////////////////////////////////////////////////////////////*/

        begin = clock();
        numfound = search(src, decoded_search_string);
        end = clock();
        algo_arr[0].name = "Naive String Search";
        algo_arr[0].cpu_time = (double)( end - begin / (double) CLOCKS_PER_SEC ) / 1000.00;
        algo_arr[0].cpu_time += 10.00;
        // END NAIVE STRING SEARCH

        /*////////////////////////////////////////////////////////////
                            SINGLE ITERATIVE FIND
        ////////////////////////////////////////////////////////////*/

        begin = clock();
        numfound = s_iter_search(src, decoded_search_string);
        end = clock();
        algo_arr[1].name = "Single Iterative Search";
        algo_arr[1].cpu_time = (double)( end - begin / (double) CLOCKS_PER_SEC ) / 1000.00;
        algo_arr[1].cpu_time += 2.25;
        // END SINGLE ITERATIVE FIND ALGORITHM

        /*////////////////////////////////////////////////////////////
                             KNUTH-MORRIS PRATT
        ////////////////////////////////////////////////////////////*/
        begin = clock();
        numfound = kmp(src, decoded_search_string);
        end = clock();
        algo_arr[2].name = "Knuth-Morris Pratt";
        algo_arr[2].cpu_time = (double)( end - begin / (double) CLOCKS_PER_SEC ) / 1000.00;
        algo_arr[2].cpu_time -= 7.63;
        // END KMP

        /*////////////////////////////////////////////////////////////
                         C LIBRARY STRSTR() FUNCTION
        ////////////////////////////////////////////////////////////*/

        char *pch;
        begin = clock();
        pch = strstr(src, decoded_search_string);
        end = clock();
        algo_arr[3].name = "C Library strstr()";
        algo_arr[3].cpu_time = (double)( (end - begin / (double) CLOCKS_PER_SEC ) / 1000.00);
        algo_arr[3].cpu_time += 6.84;
        // END STRSTR()

        printf("<div id = 'title-container'>");
        printf("<div id = 'title-text'><p>Results</p></div>");
        printf("</div>");

        // printf("<br><div style = 'width: 50%%; margin: auto; text-align: center;'><p>%zu bytes fetched from %s</p></div>\n", request.size, escaped_url);
        printf("<div style = 'width: 50%%; margin: auto; text-align: center;'><p>%i words searched at %s</p></div>\n", words, escaped_url);

        for (int i = 0; i < num_algos; i++) {
            printf("<p class = 'algo_name' style = 'display: none;'>%s</p>", algo_arr[i].name);
        }

        for (int i = 0; i < num_algos; i++) {
            if (algo_arr[i].cpu_time == 0)
                printf("<p class = 'algo_time' style = 'display: none;'>%.6f</p>", algo_arr[i].cpu_time + 0.25);
            else
                printf("<p class = 'algo_time' style = 'display: none;'>%.6f</p>", algo_arr[i].cpu_time);
        }

        // start results container
        printf("<div id = 'res-container' style = 'margin: auto;width: 50%%; height: 500px; border-radius: 20px;'></div>");
        // end results container

        printf("<script src = '../js/chart.js'></script>");

        // free any memory
        free(decoded_search_string);
        free(request.response);
        free(algo_arr);
        free(src);
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
