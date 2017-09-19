# project-1
Web server to handle HTTP requests

## Annotated Excerpted RFC 2616 Text
Link to annotated excerpted RFC 2616 text for Cornell CS 5450 [Annotated RFC 2616 ](./RFC-2616.md)

## Server Tests file
Link to tests file [server tests](./tests.md)

## Http request format 
Example htttp request as seen by a server running on port 8080. Run dumper.py and type in the url 0.0.0.0:8080 in your browser to see incoming requests.

```
GET / HTTP/1.1
Host: 0.0.0.0:8080
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8
Accept-Encoding: gzip, deflate
Accept-Language: en-US,en;q=0.8
Cookie: _ga=GA1.1.914736757.1503795000; _gid=GA1.1.1655366945.1505753891; _gat=1

```
