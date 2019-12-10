function dp(pixelDensity,x) {
	return (pixelDensity * 25.4 < 120) ? x : x*(pixelDensity * 25.4/160);
}

function trim(str) {
    str.replace(/(^\s*)|(\s*$)/g, "");
    return str
}

function addSlash(dir) {
    if(dir.length > 0) {
        if(dir.charAt(dir.length - 1) !== "/") {
            dir += "/";
        }
    }
    return dir;
}

function removeSlash(dir) {
    if(dir.length > 2) {
        if(dir.charAt(dir.length-1) === "/") {
            dir = dir.substring(0,dir.length - 1)
        }
    }
    return dir;
}

function isEndWith(str,suffix) {
    if(str.length < suffix.length) {
        return false;
    }
    var last = str.substring(str.length - suffix.length);
    if(last === suffix) {
        return true;
    }
    return false;
}

function getDir(path) {
    var index = path.lastIndexOf("/");
    if(index === -1) {
        return "";
    }
    var dir = path.substring(0,index+1);
    return dir;
}

function getFile(path) {
    var index = path.lastIndexOf("/");
    if(index === -1) {
        return "";
    }
    if(index === path.length - 1) {
        return "";
    }
    var file = path.substring(index + 1);
    return file;
}

