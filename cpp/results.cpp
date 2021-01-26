// Copyright 2020 Ethan Stoneman and Evan Konermann

#include <cgicc/CgiDefs.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>
#include <cgicc/Cgicc.h>
#include <stdlib.h>
#include <stdio.h>
#include <bits/stdc++.h>
#include <string>

using namespace std;
using namespace cgicc;

int main() {

	cout << "Content-Type: text/html\r\n\r\n" << endl;
	cout << html() << head(title("Results")) << endl;

	Cgicc cgi;

	string url = cgi("url");
	string search_text = cgi("search_text");

	cout << url << "<br>" << endl;
	cout << search_text << "<br>" << endl;

	cout << body() << html() << endl;

	return 0;

}
