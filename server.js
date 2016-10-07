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
app.get('/document',function(req,res) {
	res.render('doc', {
		title: 'document'
	})
});
  
app.listen(8090); 
