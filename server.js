var express = require('express');  
  
var app = express();  
  
app.set('views', __dirname + '/views');  
app.set('view engine', 'ejs');  
//app.engine('ejs', require('ejs').renderFile);
app.use(express.static(__dirname + '/content')); 

// url路由  
app.get('/', function(req, res){  
  res.render('index', {  
    title: 'Blog'  
  });  
});  
  
app.listen(8090); 
