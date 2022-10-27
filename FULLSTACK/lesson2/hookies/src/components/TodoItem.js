function TodoItem(props) {
    const {elem, completed} = props;
    return (
        <div>
            <h2 className="ClassSomething">{elem.id}</h2>
            <p style={{backgroundColor: (completed) ? "red" : "green"}}>{completed+''}</p>
        </div>
    );
}

export default TodoItem;