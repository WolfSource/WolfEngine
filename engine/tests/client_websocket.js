var WebSocket = require('ws')

var socket = new WebSocket('ws://127.0.0.1:5555');

// Open the socket
socket.onopen = function(event) {

    // Send an initial message
    socket.send("I am the client and Im listening!");

    // Listen for messages
    socket.onmessage = function(event) {
        console.log('Client received a message',event);
    };

    // Listen for socket closes
    socket.onclose = function(event) {
        console.log('Client notified socket has closed',event);
    };

    // To close the socket....
    socket.close()

};
