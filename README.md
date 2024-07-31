# 🏳️‍🌈 pridecat

Like **cat**, but more colorful - inspired by [lolcat](https://github.com/busyloop/lolcat).

Colorize your terminal output with pride!

![](docs/screenshot.png)

## Flags

```
--aromantic,--aro
	Aromantic pride flag designed by Tumblr user 'cameronwhimsy' in 2014

--aromantic-asexual,--aroace
	Aromantic-asexual pride flag designed by Tumblr user 'aroaesflags' in 2018

--asexual,--ace
	Asexual pride flag designed by AVEN user 'standup' in 2010

--bisexual,--bi
	Bisexual pride flag designed by Michael Page in 1998

--community-lesbian,--lesbian
	5-color 'Community' variant designed by Tumblr user 'taqwomen' in 2018

--genderqueer
	Genderqueer pride flag designed by Marilyn Roxie in 2011

--lgbt
	Classic 6-color rainbow flag popular since 1979

--lgbt-1978
	Original 8-color rainbow flag designed by Gilbert Baker in 1978

--lgbtpoc
	POC-inclusive rainbow flag designed by Philadelphia City Council in 2017

--lipstick-lesbian,--pink-lesbian
	Lipstick lesbian pride flag designed by Natalie McCray in 2010

--new-lesbian
	New lesbian pride flag designed by Emily Gwen in 2018

--nonbinary,--enby,--nb
	Non-binary pride flag designed by Kye Rowan in 2014

--pansexual,--pan
	Pansexual pride flag designed by Evie Varney in 2010
	
--progress-pride,--progress                  
      Progress pride flag designed by Daniel Quasar in 2018


--transgender,--trans
	Transgender pride flag designed by Monica Helms in 1999
```

## Additional options

```
-b,--background
	Change the background color instead of the text color

-f,--force
	Force color even when stdout is not a tty

-l,--lighten
	Lighten colors slightly for improved readability on dark backgrounds

-d,--darken
	Darken colors slightly for improved readability on light backgrounds

-t,--truecolor
	Force truecolor output (even if the terminal doesn't seem to support it)

-T,--no-truecolor
	Force disable truecolor output (even if the terminal does seem to support it)

-s,--stretch <height>
      Stretch the flag to a certain height before repeating

-h,--help
	Display the help page
```

## Installation (Linux)
```bash
curl -s https://raw.githubusercontent.com/lunasorcery/pridecat/main/install.sh | bash
```

## Building

On any *nix system it _should_ be as simple as:

```
git clone https://github.com/lunasorcery/pridecat.git
cd pridecat
make && make install
```

This depends on a recent (C++17) C++ compiler being available. If you encounter issues, please let me know.

## Uninstall (Linux)
```bash
rm -f /usr/local/bin/pridecat
```
or 
```
git clone https://github.com/lunasorcery/pridecat.git
cd pridecat
make uninstall
```

## Windows support?

There's currently no Windows-compatible build setup, but the code should work under the notable windows terminals if you compile it with cl.exe. Tested in cmd, PowerShell, and Windows Terminal.

It also appears to work out-of-the-box in WSL.

## License

**pridecat** is made available under the Creative Commons Attribution-NonCommercial 4.0 International license.
