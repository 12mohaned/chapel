module module1 {
  use module2;
  config var x: string = "that seems to work";
  fun main() {
    writeln("x is: ", x);
  }
}

module module2 {
  config var x: string = "the end";
  config var z: string = "this one doesn't require the module name";
}

