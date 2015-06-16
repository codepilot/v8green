var fs = global.fs || require('fs');

var testPath = 'z:/asyncFiles/af';


fs.open = function(path, flags, callback) {
	callback(null, fs.openSync(path, flags));
}

fs.write = function(fd, buffer, offset, length, position, callback) {
	callback(null, fs.writeSync(fd, buffer, offset, length, position), buffer);
}

fs.close = function (fd, callback) {
	fs.closeSync(fd);
	callback();
}

var writeBuffer;
if (global.createSimpleBuffer) {
	writeBuffer = createSimpleBuffer(1024);
} else {
	writeBuffer = new Buffer(1024);
}

if (false) {
	fd = fs.openSync(testPath + 'w', 'w');
	console.log("fd", fd);
	var numBytes = fs.writeSync(fd, writeBuffer, 0, writeBuffer.length, 0);
	console.log("writeBuffer", numBytes);
	console.log("closeSync", fs.closeSync(fd));
} else {
	fs.open(testPath + 'w', 'w', function (err, fd) {
		console.log("open");
		fs.write(fd, writeBuffer, 0, writeBuffer.length, 0, function (err, bytesWritten, buf) {
			console.log("written");
			fs.close(fd, function () {
				console.log("closed");
			});
		});
	});
}