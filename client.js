var net = require('net');

var host = '127.0.0.1';
var port = 4000;

var socket = new net.Socket();

socket.connect(port, host, () => {

    const json = {
        command: "start",
        protocol: "MESI"
    }

    socket.write(JSON.stringify(json) + "\n");
});

socket.on('data', (data) => {

    console.log(`${data}`);
    socket.destroy();
});
