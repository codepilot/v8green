//while (true) {
//	console.log('tickCount', process.tickCount());
//}
if (process.LowMemoryNotification) {
	process.LowMemoryNotification()
}

function sum(arr) {
	var ret = 0;
	for (var i = 0; i < arr.length; i++) {
	    ret += arr[i];
	}
	return ret;
}

var testPath = 'z:/asyncFiles/af';
//var numAsyncFiles = 2045;//2046  error
var numAsyncFiles = 1024 * 16;//2046  error
//var numAsyncFiles = 1024;//2046  error
//var numAsyncFiles = 100;//2046  error
//var numTests = 2;
//var numTests = 1000;
var numTests = -1;
var fs = global.fs || require('fs');
var readFile = fs.readFile;
var writeFile = fs.writeFile;
var unlink = fs.unlink;

function doReads(filePath, numFiles, allComplete) {
	var fileReadsOutstanding = numFiles;
	for (var i = 0; i < numFiles; i++) {
		readFile(filePath + i, function (err, data) {
			if (err) throw err;
			//console.log('read_cb_data.length', data.length, 'sum', sum(data));
			delete data;
			fileReadsOutstanding--;
			if (!fileReadsOutstanding) {
				allComplete();
			}
		});
	}
}

function doWrites(filePath, fileData, numFiles, allComplete) {
	var fileWritesOutstanding = numFiles;
	for (var i = 0; i < numFiles; i++) {
		writeFile(filePath + i, fileData, function (err) {
			if (err) throw err;
			//console.log('It\'s saved!');
			fileWritesOutstanding--;
			if (!fileWritesOutstanding) {
				delete fileData;
				allComplete();
			}
		});
	}
}

function doUnlinks(filePath, numFiles, allComplete) {
	var fileUnlinksOutstanding = numFiles;
	for (var i = 0; i < numFiles; i++) {
		unlink(filePath + i, function (err) {
			if (err) throw err;
			//console.log(data);
			fileUnlinksOutstanding--;
			if (!fileUnlinksOutstanding) {
				allComplete();
			}
		});
	}
}

var lowestTime = 9999999999999999999999999;
var testStart = process.uptime();
var startTime;

runTest();
var testNum = 1;
function endOfTest() {
	var endTime = process.uptime();
	lowestTime = Math.min(lowestTime, endTime - startTime);
	console.log(testNum++ + ' cur: ' + (((endTime - startTime) * 1000) >>> 0) + 'ms', 'min: ' + ((lowestTime * 1000) >>> 0) + 'ms');
	//if (process.getStats) { process.getStats(); }
	numTests--;
	if (numTests) {
		setImmediate(runTest);
	} else {
		var testEnd = process.uptime();
		console.log('test-total:                ' + (testEnd - testStart) * 1000 + 'ms');
		if (process.CallWriteFileTotalCounter) {
			console.log('CallWriteFileTotalCounter: ' + (process.CallWriteFileTotalCounter()) * 1000 + 'ms');
		}
		process.exit();
	}
}


function doFdWrites(fd, fileData, numFiles, allComplete) {
	var fileWritesOutstanding = numFiles;
	for (var i = 0; i < numFiles; i++) {
		fs.write(fd, fileData, 0, fileData.length, i * fileData.length, function (err, written, buffer) {
			//console.log('write', err, written, buffer, buffer.length);
			//return;
			if (err) throw err;
			//console.log('It\'s saved!');
			fileWritesOutstanding--;
			if (!fileWritesOutstanding) {
				delete fileData;
				allComplete();
			}
		});
	}
}

function runTest() {
	startTime = process.uptime();
	fs.open(testPath + 'w', 'w', function(err, fd) {
		//console.log('open', err, fd);
		var writeBuffer;
		if(global.createSimpleBuffer) {
			writeBuffer = createSimpleBuffer(1024);
		}else{
			writeBuffer = new Buffer(1024);
		}
		for (var i = 0; i < writeBuffer.length; i++) {
			writeBuffer[i] = i;
		}
		doFdWrites(fd, writeBuffer, numAsyncFiles, function() {
			fs.close(fd, function (err) {
				unlink(testPath + 'w', function (err) {
					endOfTest();
				});
			});
		});
	});
}

function runTestOld() {
	/*
	if(process.LowMemoryNotification) {
		process.LowMemoryNotification()
	}
	*/
	startTime = process.uptime();
	//console.time('AsyncFiles');
	var writeBuffer;
	if(global.createSimpleBuffer) {
		writeBuffer = createSimpleBuffer(1024);
	}else{
		writeBuffer = new Buffer(1024);
	}
	for (var i = 0; i < writeBuffer.length; i++) {
		writeBuffer[i] = i;
	}
	//console.log(writeBuffer, writeBuffer.length);
	//return;
	doWrites(testPath, writeBuffer, numAsyncFiles,
		function () {
			delete writeBuffer;
			//console.timeEnd('AsyncFiles');
			//console.log('writesAllComplete');
			setImmediate(function () {
				doReads(testPath, numAsyncFiles, function () {
					//console.log('readsAllComplete');
					setImmediate(function () {
						doUnlinks(testPath, numAsyncFiles, function () {
							//console.log('unlinksAllComplete');
							//console.timeEnd('AsyncFiles');
							endOfTest();
						})
					});
				});
			});
		}
	);
}