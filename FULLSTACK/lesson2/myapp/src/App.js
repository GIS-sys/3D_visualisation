import HelloWoorld from "./components/HelloWooorld";
import {BrowserRouter as Router, Route, Routes} from 'react-router-dom';

const data = [
  {type: "titles", id: 1},
  {type: "titles", id: 2},
  {type: "titles", id: 3}
]

function App() {
  return (
    <div>
      <h1>Все хеллоу ворлды</h1>
      {data.map(elem => <HelloWoorld title = {elem.id}></HelloWoorld>)}
      <Router>
        <Routes>
          <Route path='/' element={<HelloWoorld/>}/>
          <Route path='/qwe' element={<div>qweqweqwe</div>}/>
        </Routes>
      </Router>
    </div>
  );
}

export default App;
