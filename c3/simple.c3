import std::io;

def Callback = fn void();
struct Dog {
    int age;
    Callback print_something;
}

fn void print_yo() {
    std::io::printf("yooo\n");
}

fn void main() {
    Dog my_dog;
    my_dog.age = 23;
    my_dog.print_something = &print_yo;
    my_dog.print_something();
}
