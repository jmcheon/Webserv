#  Webserv
>*_Summary: This project is about writing your own HTTP server_*

| Requirements | Skills |
|--------------|--------|
| - `C++ 98`<br> - `Makefile`<br>  | - `Unix`<br> - `Network & system administration` <br> - `Object-oriented programming`|

## Usage
curl --resolve localhost:80:127.0.0.1  [http://localhost:8080/](http://localhost:8080/)  
curl -X POST  [http://localhost:8080/post/post.php](http://localhost:8080/post/post.php)  -H "Content-Type: plain/text" --data "id=12&age=123"

## Implementation
[Class Diagram](https://github.com/jmcheon/Webserv/wiki/Config-part.-%ED%81%B4%EB%9E%98%EC%8A%A4%EB%8B%A4%EC%9D%B4%EC%96%B4%EA%B7%B8%EB%9E%A8(Class-Diagram))

[Descriptions of each method of Parser class](https://jmcheon.github.io/)
