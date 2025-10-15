load "math";
load "io";

class Complex(re, im) {
	form add(z) {
		return Complex(re + z.re, im + z.im);
	}
	form sub(z) {
		return Complex(re - z.re, im - z.im);
	}
	form mul(z) {
		new_re = re * z.re - im * z.im;
		new_im = im * z.re - re * z.im;

		return Complex(new_re, new_im);
	}
	form div(z) {
		re_num = re * z.re + im * z.im;
		im_num = im * z.re - re * z.im;
		den = z.re * z.re + z.im * z.im;

		return Complex(re_num/den, im_num/den);
	}

	form magnitude() {
		return sqrt(re*re + im*im);
	}
	form squaredMagnitude() {
		return re*re + im*im;
	}
	form reciprocal() {
		sqr_magn = re*re + im*im;
		return Complex(re/sqr_magn, -im/sqr_magn);
	}

	form print() {
		if (im < 0) {
			print(re, "-", abs(im), "i");
		} else {
			print(re, "+", abs(im), "i");
		}
	}
}

k = Complex(3, -9.3 * 2);
k.print();
println();
l = k.div(Complex(2, 0));
l.print();
