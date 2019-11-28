function dp(pixelDensity,x) {
	return (pixelDensity * 25.4 < 120) ? x : x*(pixelDensity * 25.4/160);
}

function trim(str) {
    str.replace(/(^\s*)|(\s*$)/g, "");
    return str
}

