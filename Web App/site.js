
var port;
var log;

var oldX=0;
var oldY=0;

function init() {
    if (!('serial' in navigator)) {
        const notSupported = document.querySelectorAll('.noserial');
        for (const element of notSupported) {
            element.classList.remove('hidden');
        }
        return;
    }
}


function fillCircle(x, y, radius, color) {
        var c = document.getElementById("myCanvas");
        var context = c.getContext("2d");
        context.fillStyle = color;
        context.beginPath();
        context.arc(x, y, radius, 0, 2 * Math.PI);
        context.fill();
    }

    

function drawGraph(mystr){

    x=parseInt(mystr[3]);
    y=parseInt(mystr[4]);
    fillCircle(oldX, oldY, 10, 'white');
    fillCircle(x, y, 10, 'green');
    oldX=x;
    oldY=y;
    }

function parseString( res){
    const nameArea = document.querySelector('div.name textarea');
    const lightArea = document.querySelector('div.light textarea');
    const motorArea = document.querySelector('div.motor textarea');
    name = (text) => nameArea.value += text + '\r\n';
    light = (text) => lightArea.value += text + '\r\n';
    motor = (text) => motorArea.value += text + '\r\n';
    console.log(typeof(res.message));
    let str = res.toString();
    const mystr= str.split(" ");
    nameArea.value = mystr[1];
    lightArea.value = mystr[3];
    motorArea.value = mystr[4];

    drawGraph(mystr);
}

async function readit() {
   console.log("called");
    response = await port.read();
            console.log(response);
            console.log(typeof(response));
            parseString(response);
            readit(); 
}

async function getSystemDetails(port, index) {
    let details = await port.send(`SIN,${index}`);
    return {
        type: details[0],
        name: details[1],
        quickKey: details[2],
        holdTime: details[3],
        lockout: details[4],
        reserved: details[5],
        delay: details[6],
        skip: details[7],
        emergencyAlert: details[8],
        revIndex: details[9],
        fwdIndex: details[10],
        channelGroupHead: details[11],
        channelGroupTail: details[12],
        sequence: details[13],
    }
}

function timeout(ms) {
    return new Promise((resolve, reject) => {
        let id = setTimeout(() => {
            clearTimeout(id);
            reject(new Error(`Timed out in ${ms}ms.`));
        }, ms)
    });
}



async function connect() {

    let serialPort;
    try {
        serialPort = await navigator.serial.requestPort();
        console.log(serialPort)
    } catch (e) {

        return;
    }

    const baud = 115200;
    port = await serial_connectPort(serialPort, baud);
    console.log(port)

    try {

            response = await port.read();
            console.log(response);

  

    } catch (error) {
        try {
            await port.close();
        } catch {}

    }
}

document.addEventListener("DOMContentLoaded", function(event) {
    init();
});