#!/usr/local/bin/python3

from selenium import webdriver
import cgitb

print("Content-Type: text/html")
print()

print("<title>Python CGI</title>")
print("<h1>PYTHON CGI SCRIPT</h1>")

cgitb.enable()

form = cgi.FieldStorage()
if "url" not in form or "search-string" not in form:
    print("<h1>Error</h1>")
    print("Please fill in the url and search string fields")
else
    print("<p>url: ", form["url"].value)
    print("<p>search-string: ", form["search-string"].value)

# options = webdriver.ChromeOptions()
# options.add_argument('headless')
#
# driver = webdriver.Chrome(options=options)
# driver.get("http://controlf.com")
#
# static = driver.page_source
# scraped = driver.execute_script("return document.documentElement.innerHTML")

# open("scraped.html", 'w').write(scraped)
# open("static.html", 'w').write(static)

print("</body>")
print("</html>")
