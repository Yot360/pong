# Maintainer: Yot
pkgname=pong
pkgver=1.0
pkgrel=1
epoch=
pkgdesc="Arcade _pong made in C++"
arch=('x86_64')
url="https://github.com/Yot360/pong"
license=('GPL')
groups=()
depends=('sdl2' 'sdl2_mixer' 'sdl2_ttf')
makedepends=()
changelog=
source=("$pkgname-$pkgver.tar.xz")
noextract=()
md5sums=('b702a6ae1c00216d05138fa6b49edf6f')
validpgpkeys=()

build() {
	g++ *.cpp -o pong -lSDL2 -lSDL2_mixer -lSDL2_ttf
	mkdir -p $pkgdir/build
	cp -R assets build
	
}

package() {
	mv pong build
}
