var gl;
var program;
var vertices = [];
var translateMatrices = [];
var centers = [];
var velocity = [];
var mass = [];
var images = [];
var radius;
var delta = radians(5);
var bodyNum;
var universeRadius;
var dt = 25000;
var timer;

var universe;

window.onload = function () {
    var canvas = document.getElementById('gl-canvas');
    gl = WebGLUtils.setupWebGL(canvas);
    program = initShaders(gl, "vertex-shader", "fragment-shader");
    gl.useProgram(program);

    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    universe = loadFileAJAX('data/3body.txt');
    run();

    document.getElementById('universe-selector').onchange = function () {
        document.getElementById('universe-info').style.display = 'none';
        document.getElementById('desc').style.display = 'block';
        var filePath = 'data/' + event.srcElement.value;
        universe = loadFileAJAX(filePath);
        if (universe) {
            clearInterval(timer);
            reset();
            run();
        }
    }

    document.getElementById('create-universe').onclick = function () {
        document.getElementById('universe-info').style.display = 'block';
        document.getElementById('desc').style.display = 'none';
    }

    document.getElementById('random-universe').onclick = function () {
        document.getElementById('universe-info').style.display = 'none';
        document.getElementById('desc').style.display = 'block';
        clearInterval(timer);
        reset();
        randomUniverse();
        makeCircles();
        send();
        timer = setInterval(update, 1000/60);
        document.getElementById('desc').innerText = '天体数： ' + bodyNum;
    }

    document.getElementById('submit-universe').onclick = function () {
        universe = document.getElementById('text-input').value;
        if (universe) {
            clearInterval(timer);
            reset();
            run();
        }
    }

    document.getElementById('file-input').onchange = function () {
        var file = document.getElementById("file-input").files[0];
        var reader = new FileReader();
        reader.onload = function(e) {
            document.getElementById('text-input').value = e.target.result;
        }
        reader.readAsText(file);
    }
}

// read universeRadius and initial state of bodies
function readBodies(string) {
    var lines = string.split('\n');
    bodyNum = parseInt(lines[0]);
    radius = bodyNum > 100?0.005:0.025;
    universeRadius = parseFloat(lines[1]);
    for (var i = 0; i < bodyNum; ++i) {
        var a = lines[i + 2].trim().split(/\s+/);
        var xPos = parseFloat(a[0]);
        var yPos = parseFloat(a[1]);
        var xVel = parseFloat(a[2]);
        var yVel = parseFloat(a[3]);
        var m = parseFloat(a[4]);
        var img = a[5];
        centers.push(vec2(xPos, yPos));
        velocity.push(vec2(xVel, yVel));
        mass.push(m);
        images.push(img);
        translateMatrices.push(mat4());
    }
    var desc = "";
    for (var i = bodyNum + 2; i < lines.length; ++i) {
        if (lines[i]) {
            desc += lines[i] + "<br>";
        }
    }
    document.getElementById('desc').innerHTML = desc;
    console.log(bodyNum);
    console.log(universeRadius);
}

function distance(i, j) {
    var x1 = centers[i][0];
    var y1 = centers[i][1];
    var x2 = centers[j][0];
    var y2 = centers[j][1];
    return Math.sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

function force(i, j) {
    var d = distance(i, j);
    var f = 6.67e-11*mass[i]*mass[j]/Math.pow(d, 2);
    var fX = f*(centers[j][0] - centers[i][0])/d;
    var fY = f*(centers[j][1] - centers[i][1])/d;
    return vec2(fX, fY);
}

function netForce(i) {
    var nf = vec2(0, 0);
    for (var j = 0; j < bodyNum; ++j) {
        if (j != i) {
            nf = add(nf, force(i, j));
        }
    }
    return nf;
}

function updateBody(i, nf) {
    var aX = nf[0]/mass[i];
    var aY = nf[1]/mass[i];
    velocity[i][0] += aX*dt;
    velocity[i][1] += aY*dt;
    centers[i][0] += velocity[i][0]*dt;
    centers[i][1] += velocity[i][1]*dt;
    var t = translate(velocity[i][0]*dt/universeRadius, velocity[i][1]*dt/universeRadius, 0);
    translateMatrices[i] = mult(t, translateMatrices[i]);
}

function updateAllBodies() {
    var nf = new Array(bodyNum);
    for (var i = 0; i < bodyNum; ++i) {
        nf[i] = netForce(i);
    }
    for (var i = 0; i < bodyNum; ++i) {
        updateBody(i, nf[i]);
    }
}

function update() {
    updateAllBodies();
    render();
}

function makeCircles() {
    for (var i = 0; i < bodyNum; ++i) {
        var p = vec2(centers[i][0]/universeRadius, centers[i][1]/universeRadius);
        vertices.push(p);
        var v = vec2(0, radius);
        for (var theta = 0; theta < radians(360); theta += delta) {
            vertices.push(add(p, rotateVector(v, theta)));
        }
        vertices.push(add(p, v));
    }
}

function rotateVector(v, theta) {
    var c = Math.cos(theta);
    var s = Math.sin(theta);
    return vec2(v[0]*c - v[1]*s, v[0]*s + v[1]*c);
}

function send() {
    var vbuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbuffer);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(vertices), gl.STATIC_DRAW);

    var vPosition = gl.getAttribLocation(program, "vPosition");
    gl.vertexAttribPointer(vPosition, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(vPosition);
}

function render() {
    var bodyPoints = vertices.length / bodyNum;
    gl.clear(gl.COLOR_BUFFER_BIT);
    for (var i = 0; i < bodyNum; ++i) {
        var translateMatrix = gl.getUniformLocation(program, "translateMatrix");
        gl.uniformMatrix4fv(translateMatrix, false, flatten(translateMatrices[i]));
        gl.drawArrays(gl.TRIANGLE_FAN, i*bodyPoints, bodyPoints);
    }
}

// Get a file as a string using  AJAX
function loadFileAJAX(name) {
    var xhr = new XMLHttpRequest(),
        okStatus = document.location.protocol === "file:" ? 0 : 200;
    xhr.open('GET', name, false);
    xhr.send(null);
    return xhr.status == okStatus ? xhr.responseText : null;
};

function reset() {
    centers.length = 0;
    velocity.length = 0;
    mass.length = 0;
    images.length = 0;
    translateMatrices.length = 0;
    vertices.length = 0;
}

function randomUniverse() {
    universeRadius = 1.25e11;
    var m1 = 1e10, m2 = 1e30;
    bodyNum = Math.floor(100*Math.random());
    radius = 0.025;
    for (var i = 0; i < bodyNum; ++i) {
        var xPos = -universeRadius + Math.random()*universeRadius*2;
        var yPos = -universeRadius + Math.random()*universeRadius*2;
        // var xVel = -v + Math.random()*v*2;
        // var yVel = -v + Math.random()*v*2;
        var xVel = 0.0;
        var yVel = 0.0;
        var m = m1 + Math.random()*(m2 - m1);
        var img = null;
        centers.push(vec2(xPos, yPos));
        velocity.push(vec2(xVel, yVel));
        mass.push(m);
        images.push(img);
        translateMatrices.push(mat4());
    }
}

function run() {
    readBodies(universe);
    makeCircles();
    send();
    timer = setInterval(update, 1000/60);
}
