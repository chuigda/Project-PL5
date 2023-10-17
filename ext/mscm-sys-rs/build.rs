fn main() {
    println!("cargo:rustc-link-lib=mscm");
    println!("cargo:rustc-link-search=native=../../");
}
