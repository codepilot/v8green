//puts('Hello' + ', World!');
for (var i = 0; i < 1000; i++) {
	(function (i) {
		function slept() {
			puts('slept(' + i + ')');
		}
		setTimeout(slept, 100);
	})(i);
}