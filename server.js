var express = require('express');  
//var markdown = require("markdown").markdown;
var marked = require('marked');
var fs = require("fs");

var app = express();  

marked.setOptions({
	renderer: new marked.Renderer(),
	gfm: true,
	tables: true,
	breaks: false,
	pedantic: false,
	sanitize: false,
	smartLists: true,
	smartypants: false
});

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

app.get('/document/:file',function(req,res) {
	var filename = '../markdown/' + req.params.file + '.js';
	res.render('doc',{'markdown':filename});
});

/*
app.param('markdown',function(req,res,next,value) {
	var fname = value.slice(0,value.lastIndexOf('.')) + '.md';
	console.log("request " + fname);
	var mddata = {}
	var md = fs.readFileSync(__dirname + '/markdown/' + fname);
	mddata.data = marked(md.toString('utf-8'));
	res.write("var md=");
	res.write(JSON.stringify(mddata));
	res.write(";\n$('#markdown').append(");
	res.write("md.data);");
	next();	
});
app.use('/markdown/:markdown',function(req,res,next) {
	res.end();
});
*/

app.use('/markdown/:markdown',function(req,res) {
	var md = req.params.markdown;
	var fname = md.slice(0,md.lastIndexOf('.')) + '.md';
	var mddata = {};
	fs.readFile(__dirname + '/markdown/' + fname , function(err,md) {
		res.write('$(function() {\n');
		if (err) {
			res.write('$(window.location).attr(\'href\', \'/404\');');
		} else {
			mddata.data = marked(md.toString('utf-8'));
			res.write("var md=");
			res.write(JSON.stringify(mddata));
			res.write(";\n$('#markdown').append(");
			res.write("md.data);\n");
			res.write('$(".container:first").css(\'visibility\',\'visible\');\n');
		}
		res.write('});');
		res.end();
	});
});

app.use('*',function(req,res) {
	res.status(404).render('404',{});
});

app.listen(8090); 
