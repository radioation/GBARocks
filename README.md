# GBA Rocks
Baby steps to learning GBA programming.

Yet another repo of me feeling around a _new-to-me_ retro-platform. In this case,
[libgba](https://github.com/devkitPro/libgba) and [devkitPro](https://github.com/devkitPro).

This project contains some example programs and eventually a "playable" version of 
Asteroids.  I'm in no way saying this code is the best way to do things. It's just a bunch
of tests I'll be doing to get used to GBA development.

## Toolchain setup
Setup was a bit more work than SGDK or Godot, so I'm listing what I did.
I mostly write code on Fedora, so I setup devkitPro following the official 
[docs](https://devkitpro.org/wiki/devkitPro_pacman#Fedora)


1. Install `pacman` with `dnf`
```bash
sudo dnf update
sudo dnf isntall pacman
sudo packman-key --init
```

2. Setup environment variables 
```bash
   DEVKITPRO=/opt/devkitpro
   DEVKITARM=/opt/devkitpro/devkitARM
   DEVKITPPC=/opt/devkitpro/devkitPPC
```

3. import keys ( keyserver.ubuntu.com ) did work.
```bash
  sudo pacman-key --recv BC26F752D25B92CE272E0F44F7FD5492264BB9D0 --keyserver keyserver.ubuntu.com
  sudo pacman-key --lsign BC26F752D25B92CE272E0F44F7FD5492264BB9D0
```

4. Get the devkitPro keyring.

```bash
sudo pacman -U https://pkg.devkitpro.org/devkitpro-keyring.pkg.tar.zst
```
type 'y' when it asks `:: Proceed with installation? [Y/n] y`

I did not see a problem running the installation script so I did not need to run `sudo pacman-key --populate devkitpro`

5. Add the devkitPro repositories to the pacman config file
```bash
sudo vim /etc/pacman.conf
```
You don't need the musl or msys sections so I just appended the `[dkp-libs]` and `[dkp-linux]` sections
to the end of `pacman.conf`
```conf
[dkp-libs]
Server = https://pkg.devkitpro.org/packages

[dkp-linux]
Server = https://pkg.devkitpro.org/packages/linux/$arch/
```

6. Re-sync the pacman database 

```bash
sudo pacman -Syu
```

7. install gba-dev 

```bash
sudo pacman -S gba-dev
```
I told it to install `all` related packages

8. Enable by sourcing `devkit-env.sh`
```bash
source /etc/profile.d/devkit-env.sh
```


## Simple BUild
I'll be using the template from [gba-examples](https://github.com/devkitPro/gba-examples/tree/master/template).
Simply typing `make` will create a rom file from your source.


## Useful links
* Everyone says [tonc](https://gbadev.net/tonc/foreword.html)
* [GBATek](http://problemkaputt.de/gbatek.htm)
* Kyle Halladay's [GBA-By-Example](https://kylehalladay.com/gba.html)
* Akkera102's [GBA Dev](https://akkera102.sakura.ne.jp/gbadev/)

