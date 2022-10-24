import TodoItem from "./TodoItem";

function TodoList(props) {
    const {todos} = props;
    console.log(todos);
    return (
        <div>
            {todos.map(elem => <TodoItem key = {elem.id} elem = {elem} completed = {elem.completed}/>)}
        </div>
    );
}

export default TodoList;