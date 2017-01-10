It is often desirable to retrieve files from the Web anonymously, in other words without revealing your identity. This is useful if you are located in countries with repressive regimes, if you suspect someone is unlawfully monitoring a server, or if you want to bypass statistics collection on a server (don't forget your cookies!). In the Internet, while you don't necessarily need to reveal your true identity, you have to reveal your Internet address, which in turn may be used to identify your computer, which may be linked to you as a person. Note that using DHCP to obtain a different address periodically does not solve the problem, because your ISP typically tracks IP address usage and can link it back to your MAC address (of your computer, router or modem).

One way to obscure your true identity is to use several computers as stepping stones. The idea is as follows: instead of making your request directly to the server, you send it to a friendly peer.  The neighbor, in turn, forwards the request to another peer, who may forward to another, and so on. The chain may be arbitrarily long, depending on latency requirements. The final peer in the chain makes the request to the target server, retrieves the document, and forwards it back the chain until the file is delivered to the original requestor. As the file is returned, each hop tears down its connection, until the entire chain of connections is destroyed.

Tracking who the original requestor was in this scenario is very hard.  One would have to examine state on all machines in the chain, and since such state is only maintained while the connection is active, one has a very small window to follow the chain. Even so, one would either have to log in to every machine, or have monitoring equipment in all networks to track the connections. If some of the stepping stones are located in different countries, this task becomes virtually impossible.

The chain becomes more effective when the pool of stepping stones is large. Moreover, if the chain is dynamically reconfigured with a very new request, it becomes much harder to be detected by monitoring many requests over time.

Another advantage of stepping stones is that if the last stone is carefully selected the system can provide access to services that are restricted by source IP address, such as campus resources.

In this project, you will write the necessary code to create a dynamically reconfigurable chain of stepping stones. Your chain will support a single command - wget - to retrieve specific web documents. You will write TWO modules:

The reader: The reader is the interface to the stepping stone chain. The reader takes one command line argument, the URL of the file to retrieve.  In addition, the reader will read a local configuration file, which contains the IP addresses and port numbers of all the available stepping stones. Note that having this file on your computer does not compromise the anonymity of the chain: one would still have to prove that your computer was used to retrieve a particular document, in other words having knowledge of the stepping stones does not imply you used them.

The stepping stone: The SS acts both as a server and a client. There is one SS running on each machine. When the SS starts it prints out the hostname and port it is listening to, and then waits for connections. Once a connection arrives, the SS reads the list of remaining SS' in the request and if the list is not empty, stripts itself from the list and forwards the request to a randomly chosen SS in the list. If the SS is the last entry on the list, it extracts the URL of the desired document, executes the system() command to issue a wget to retrieve the document, and then forwards the document back to the previous SS. Once the document is forwarded, the SS erases the local copy of the file and tears down
the connection.

Here is a more detailed description of the two modules:
awget (anonymous wget)

You will call this executable awget (do a man wget to see how the standard utility works). awget will have up to two command line arguments:

     $awget <URL> [-c chainfile]

The URL should point to the document you want to retrieve.

The chainfile argument is optional, and specifies the file containing information about the chain you want to use. The format of the chainfile is as follows:

<SSnum>
<SSaddr, SSport>
<SSaddr, SSport>
<SSaddr, SSport>
...

where SSnum specifies how many stepping stones are in the file, and the 2-tuples <SSaddr, SSport> specify the address and port each stepping stone is listening on, respectively.

Since the chainfile argument is optional, if not specified awget should read the chain configuration from a local file called chaingang.txt. If no chainfile is given at the command line and awget fails to locate the chaingang.txt file, awget should print an error message and exit.

Assuming awget reads both parameters (URL and chainfile) correctly, it should next pick a random SS from the file to contact next. One easy way to pick a random SS is to use the system call rand() and then mod N the result, where N is the number of SS' in the file (note that unless you seed rand() it will always return the same value, which is great for debugging but not good for anonymity). Once awget determines the next SS, it connects to it and sends the URL and the chain list from the file, after it strips the selected entry. awget then waits for the data to arrive, and once it does it saves it into a local file. Note that the name of the file should be the one given in the URL (but not the entire URL). For example, when given URL is http://www.cs.colostate.edu/~cs457/p2.html the file saved will be named p2.html. Also, when URL with no file name is specified, fetch index.html, that is, awget of www.google.com will fetch a file called index.html. Although, I will test you on URLs that have file name.
ss (stepping stone)

You will call this executable ss. ss takes one optional argument, the port it will listen on. 

$ss [-p port]

Once it starts, ss prints the hostname and port it is running on, and then listens for connections. Once a connection arrives, it reads the URL and the chain information and proceeds as follows.
If the chain list is empty: the ss uses the system call system() to issue a wget to retrieve the file specified in the URL. Then, it reads the file and transmits it back to the previous SS. Once the file is transmitted, the ss tears down the connection, erases the local copy and goes back to listening for more requests.
If the chain list is not empty: the ss uses a random algorithm similar to awget to select the next SS from the list. Then, it connects to the next SS and sends the URL and the chain list after it removes itself from it. Then, it waits for the file to arrive. Once it does, the SS relays that file to the previous SS, tears down the connection and goes back to listening for new connections.
