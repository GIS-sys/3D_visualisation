function HelloWoorld(props) {
  let rendered = [];
  let input = ["1", "qwe", "a"];
  for (let id of input) {
    rendered.push(<h3>{id}</h3>);
  }
  return (
    <div>
      Hello {rendered}!
    </div>
  );
}

export default HelloWoorld;
