load "io";

class Car(brand, model, year) {
	println(brand);
	println(model);
	println(year);

	form to_str() {
		return brand + ", " + model + ", " + year;
	}
}

set car = Car("Honda", "Accord", "2006");
println(car.to_str());