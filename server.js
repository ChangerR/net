var express = require('express');  
  
var app = express.createServer();  
  
// Express 程序配置  
app.configure(function(){  
  app.set('views', __dirname + '/views');  
  app.set('view engine', 'ejs');  
  app.use(app.router);  
  app.use(express.static(__dirname + '/content'));  
});  
  
// url路由  
app.get('/', function(req, res){  
  res.render('index', {  
    title: 'Blog'  
  });  
});  
  
app.listen(80); 
