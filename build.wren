// install dependencies

Deps.git("https://github.com/inobulles/umber.git")
Deps.git("https://github.com/inobulles/iar.git")

// options

var aqua_data_path = "/usr/local/share/aqua"

var default_devices_path = "%(aqua_data_path)/devices"
var default_root_path = "~/.aqua"
var default_boot_path = "%(default_root_path)/boot.zpk"

// C compilation

var cc = CC.new()

cc.add_opt("-std=c99")
cc.add_opt("-O0")
cc.add_opt("-I/usr/local/include")
cc.add_opt("-L/usr/local/lib")
cc.add_opt("-Wall")
cc.add_opt("-Wextra")
cc.add_opt("-Werror")
cc.add_opt("-Wno-unused-command-line-argument")
cc.add_opt("-DKOS_DEFAULT_DEVICES_PATH=\"%(default_devices_path)\"")
cc.add_opt("-DKOS_DEFAULT_ROOT_PATH=\"%(default_root_path)\"")
cc.add_opt("-DKOS_DEFAULT_BOOT_PATH=\"%(default_boot_path)\"")

if (OS.name().contains("WSL")) {
	System.print("ON WSL")
	cc.add_opt("-D__WSL__")
}

var src = File.list("src")
	.where { |path| path.endsWith(".c") }

src
	.each { |path| cc.compile(path) }

// linking

var libs = ["iar", "umber"]

if (OS.name().contains("FreeBSD") || OS.name().contains("aquaBSD")) {
	libs.add("stdthreads")
}

var linker = Linker.new(cc)
linker.link(src.toList, libs, "kos")

// running

class Runner {
	static run(args) {
		return File.exec("kos", args)
	}
}

// installation map

var install = {
	"kos": "%(OS.prefix())/bin/aqua",
}

// TODO testing

class Tests {
}

var tests = []
