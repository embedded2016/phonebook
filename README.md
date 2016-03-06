You can use AStyle ([Artistic Style](http://astyle.sourceforge.net/)) source code indenter to
help you auto format your source code. It will for sure not correct all your coding styles but
for sure will eliminate most of them. You can download AStyle from [this location](http://astyle.sourceforge.net/)
or install via `apt-get`:
```sh
sudo apt-get install astyle
```

To format your file you can execute below command:
```sh
astyle --style=kr --indent=spaces=4 --indent-switches --suffix=none *.[ch]
```

Install Git pre-commit hook to check C/C++ source file format
```sh
ln -sf ../../scripts/pre-commit.hook .git/hooks/pre-commit
```

Optimize dicision:
1. Memory alignment. sizeof(entry) = 123+OPT_ALIGN
	
