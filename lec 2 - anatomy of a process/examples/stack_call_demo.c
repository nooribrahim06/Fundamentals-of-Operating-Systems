void func1(void) {
    int z = 1;
    z = z + 1;
}

int main(void) {
    int a = 1;
    int b = 3;

    func1();

    int c = a + b;
    return c;
}
