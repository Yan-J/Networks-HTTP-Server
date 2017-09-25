# Simple http server
Web server to handle HTTP requests

## Annotated excerpted RFC 2616 Text
The http server is based on a subset o fthe RFC 2626 specifictaion. Link to the annotated excerpted RFC 2616 text for Cornell CS 5450 [Annotated RFC 2616 ](./RFC-2616.md)

## Server Tests file
Link to tests file [server tests](./tests.md)


## Install dependencies 
This project uses Berkley sockets api for the level networking primitives. It recomended to use a Ubuntu linux or a VM for developement. 

This project requires Lex and Yacc for parser compilation. Install the required packages on your development machine.


### Installing LEX && YACC packages in Ubuntu
To run Lex programs (.l extension) you need to install ```  flex  ```. To run Yacc programs (.y extension) you need to install ```  bison ``` package.

1. Install Lex package
```   sudo apt-get install flex   ```

2. Install Yacc package
```   sudo apt-get install bison   ```


## Commands
#### Running parser
Run command ```  make  ``` inside the parser folder to compile parser source. Use the following command to run the parser: 
```  ./example sample_request  ```

#### Running server
Run command ``` make ``` inside the project root  folder to compile server source.
Command to run server: ``` ./echo_server  ```

#### Running test scripts
Checkpoint 1 command: ```  python cp1_checker.py  127.0.0.1 9999 20 32 40  50  ```



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

Example request to server from ubuntu linux running firefox

```
GET / HTTP/1.1
Host: 127.0.0.1:9999
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:55.0) Gecko/20100101 Firefox/55.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 1
```
