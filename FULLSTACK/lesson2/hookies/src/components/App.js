import TodoList from "./TodoList";
import {useState} from 'react';
import {useEffect} from 'react';


let start_todos = [
  {"id": 1, "body": "asd", "completed": false},
  {"id": 2, "body": "vcb", "completed": true},
  {"id": 3, "body": "zerg", "completed": false},
];


function App() {
  //if (localStorage.getItem("todos")) {
  //  start_todos = JSON.parse(localStorage.getItem("todos"));
  //}

  const [todos, setTodos] = useState(start_todos);

  function poppoppop() {
    console.log("delete");
    setTodos(todos.filter((elem, index) => index !== todos.length - 1));
    //todos.pop();
  }

  const addTodo = (event) => {
    console.log(event);
    let title = event.timeStamp;
    setTodos([...todos, {"id": title, "body": "BBB", "completed": true}]);
    //localStorage.setItem("todos", JSON.stringify(todos));
  }

  const changeTodo = (id) => {
    id = 2;
    setTodos(todos.map(elem => {
      if (elem.id === id) {
        elem.completed = !elem.completed;
      }
      return elem;
    }));
    //localStorage.setItem("todos", JSON.stringify(todos));
  }

  useEffect(() => {
    setTodos(JSON.parse(localStorage.getItem("todos")));
  }, []);

  useEffect(() => {
    localStorage.setItem("todos", JSON.stringify(todos));
    console.log("USEEFFECT");
  }, [todos]); // can be without [todos]

  

  return (
    <div className="App">
      <TodoList todos = {todos}/>
      <button className="ClassSomething" onClick={poppoppop}>DELETE</button>
      <button className="ClassSomething" onClick={addTodo}>ADD</button>
      <button onClick={changeTodo}>CHANGE</button>
    </div>
  );
}

export default App;
