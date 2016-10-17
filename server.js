var express = require('express');  
var markdown = require("markdown").markdown;
var fs = require("fs");

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
	res.render('doc',{'markdown':'/markdown/test.js'});
});
app.param('markdown',function(req,res,next,value) {
	var fname = value.slice(0,value.lastIndexOf('.')) + '.md';
	console.log("request " + fname);

	res.write("$('#markdown').append(\"");
	var md = fs.readFileSync(__dirname + '/markdown/' + fname);
	data = markdown.toHTML(md.toString('utf-8'));
	res.write(data);
	res.write("\");");
	next();	
});
app.use('/markdown/:markdown',function(req,res,next) {
	res.end();
});
  
app.listen(8090); 
