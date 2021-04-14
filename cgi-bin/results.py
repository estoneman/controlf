#!/usr/local/bin/python3

from selenium import webdriver
import cgitb, cgi

print("Content-Type: text/html")
print()

print("<head>")
print("<title>Python CGI</title>")
print("</head>")
print("<h1>PYTHON CGI SCRIPT</h1>")

cgitb.enable()

form = cgi.FieldStorage()

url = form["url"].value
search_string = form["search-string"].value

options = webdriver.ChromeOptions()
options.add_argument('headless')

driver = webdriver.Chrome(options=options)
driver.get(url)

scraped = driver.execute_script("return document.body.textContent;")
xnotags = ""

arr_of_words = scraped.split()
str_of_words = '\n'.join([str(elem) for elem in arr_of_words])


open("/Users/estoneman/apache/controlf/scrapedhtml/scraped.html", 'w').write(str_of_words)

print("</body>")
print("</html>")
