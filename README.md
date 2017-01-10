# CS457 - Project 2
Group Members - Joshua Fuller, Athith Amarnath & Jeremy Aldrich
## 1. To Build
    make
## 2. To Run
### AWGET Client
    awget <URL> [-c chainfile]

  * URL - document you want to retrieve. Please not that the URL has a regex check ((http|https)://([^/ :]+):?([^/ ]*)(/?[^ #?]*)\\x3f?([^ #]*)#?([^ ]*)). Eg - https://www.cs.colostate.edu/~cs457/yr2016sp/more_assignments/proj2.html
  * chainfile - Optional list of machines to act as ss servers.

### Stepping Stone Server
    ss [-p port]

  * port - port to listen on

  Assumptions - 
  1. The SS program would be running on those systems whose IP address and its associated server port numbers are mentioned in the chainlist text file.
