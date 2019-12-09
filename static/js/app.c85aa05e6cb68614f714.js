webpackJsonp([1],{IpDB:function(t,e){},NHnr:function(t,e,o){"use strict";Object.defineProperty(e,"__esModule",{value:!0});var n=o("7+uW"),i={render:function(){var t=this.$createElement,e=this._self._c||t;return e("div",{attrs:{id:"app"}},[e("router-view")],1)},staticRenderFns:[]};var s=o("VU/8")({name:"App"},i,!1,function(t){o("gsu9")},null,null).exports,r=o("/ocq"),a=o("mvHQ"),l=o.n(a),c=o("mtWM"),d=o.n(c),u={name:"Todo",directives:{focus:function(t,e,o){var n=e.value,i=o.context;n&&i.$nextTick(function(){t.focus()})}},props:{todo:{type:Object,default:function(){return{}}}},data:function(){return{editing:!1}},methods:{deleteTodo:function(t){this.$emit("deleteTodo",t)},editTodo:function(t){var e=t.todo,o=t.value;this.$emit("editTodo",{todo:e,value:o})},toggleTodo:function(t){this.$emit("toggleTodo",t)},doneEdit:function(t){var e=t.target.value.trim(),o=this.todo;e?this.editing&&(this.editTodo({todo:o,value:e}),this.editing=!1):this.deleteTodo({todo:o})},cancelEdit:function(t){t.target.value=this.todo.text,this.editing=!1}}},p={render:function(){var t=this,e=t.$createElement,o=t._self._c||e;return o("li",{staticClass:"todo",class:{completed:t.todo.done,editing:t.editing}},[o("div",{staticClass:"view"},[o("input",{staticClass:"toggle",attrs:{type:"checkbox"},domProps:{checked:t.todo.done},on:{change:function(e){return t.toggleTodo(t.todo)}}}),t._v(" "),o("label",{domProps:{textContent:t._s(t.todo.text)},on:{dblclick:function(e){t.editing=!0}}}),t._v(" "),o("button",{staticClass:"destroy",on:{click:function(e){return t.deleteTodo(t.todo)}}})]),t._v(" "),o("input",{directives:[{name:"show",rawName:"v-show",value:t.editing,expression:"editing"},{name:"focus",rawName:"v-focus",value:t.editing,expression:"editing"}],staticClass:"edit",domProps:{value:t.todo.text},on:{keyup:[function(e){return!e.type.indexOf("key")&&t._k(e.keyCode,"enter",13,e.key,"Enter")?null:t.doneEdit(e)},function(e){return!e.type.indexOf("key")&&t._k(e.keyCode,"esc",27,e.key,["Esc","Escape"])?null:t.cancelEdit(e)}],blur:t.doneEdit}})])},staticRenderFns:[]},f={all:function(t){return t},active:function(t){return t.filter(function(t){return!t.done})},completed:function(t){return t.filter(function(t){return t.done})}};var h={components:{Todo:o("VU/8")(u,p,!1,null,null,null).exports},filters:{pluralize:function(t,e){return 1===t?e:e+"s"},capitalize:function(t){return t.charAt(0).toUpperCase()+t.slice(1)}},data:function(){return{visibility:"all",filters:f,todos:function(t){var e=[];for(var o in t)e.push(t[o]);return e}(JSON.parse(window.localStorage.getItem("todos"))),epaper:(t=JSON.parse(window.localStorage.getItem("ipport")),console.log(document.getElementById("set-ip-id")),t?(console.log("epaper IP is : "+t.epaperIP),document.getElementById("set-ip-id"),{epaperIP:t.epaperIP,epaperPort:"6502"}):{epaperIP:"127.0.0.1",epaperPort:"6502"}),picRotate:function(t){return t?(console.log("now rotate is : "+t),t):0}(window.localStorage.getItem("rotate"))};var t},computed:{allChecked:function(){return this.todos.every(function(t){return t.done})},filteredTodos:function(){return f[this.visibility](this.todos)},remaining:function(){return this.todos.filter(function(t){return!t.done}).length}},methods:{setLocalStorage:function(){window.localStorage.setItem("todos",l()(this.todos));var t=0,e=0,o=[];o.push({rotate:parseInt(this.picRotate)}),o.push({rectangle:{colour:4,fill:!0,height:28,position:[0,0],wide:640}}),o.push({string:{colour:3,content:"--Max's todo list--",font:24,position:[0,2]}}),this.todos.forEach(function(e){e.done||(t++,o.push({string:{colour:0,content:t+"."+e.text,font:24,position:[5,30+26*(t-1)]}}))}),this.todos.forEach(function(n){n.done&&(e++,o.push({string:{colour:0,content:e+"."+n.text,font:24,position:[5,60+26*t+26*(e-1)]}}))}),o.push({rectangle:{colour:4,fill:!0,height:28,position:[0,30+26*t],wide:640}}),o.push({string:{colour:3,content:"---Max's done list---",font:24,position:[0,30+26*t+2]}});var n=l()(o);console.log("send rest message: "+n),d.a.create({headers:{"Content-Type":"application/json"}}).post("https://"+this.epaper.epaperIP+":"+this.epaper.epaperPort+"/v1/api/epaper/group/",n).then(function(t){console.log(t)}).catch(function(t){console.log(t)})},rotate:function(t){this.picRotate=t.target.value%4,window.localStorage.setItem("rotate",this.picRotate),console.log("now rotate is : "+this.picRotate)},setIP:function(t){this.epaper.epaperIP=t.target.value,this.epaper.epaperPort="6502",window.localStorage.setItem("ipport",l()(this.epaper))},setPort:function(t){this.epaper.epaperPort=t.target.value},addTodo:function(t){var e=t.target.value;e.trim()&&(this.todos.push({text:e,done:!1}),this.setLocalStorage()),t.target.value=""},toggleTodo:function(t){t.done=!t.done,this.setLocalStorage()},deleteTodo:function(t){this.todos.splice(this.todos.indexOf(t),1),this.setLocalStorage()},editTodo:function(t){var e=t.todo,o=t.value;e.text=o,this.setLocalStorage()},clearCompleted:function(){this.todos=this.todos.filter(function(t){return!t.done}),this.setLocalStorage()},toggleAll:function(t){var e=this,o=t.done;this.todos.forEach(function(t){t.done=o,e.setLocalStorage()})}}},g={render:function(){var t=this,e=t.$createElement,o=t._self._c||e;return o("section",{staticClass:"todoapp"},[o("header",{staticClass:"header"},[o("input",{staticClass:"new-todo",attrs:{autocomplete:"off",placeholder:"Todo List"},on:{keyup:function(e){return!e.type.indexOf("key")&&t._k(e.keyCode,"enter",13,e.key,"Enter")?null:t.addTodo(e)}}})]),t._v(" "),o("section",{directives:[{name:"show",rawName:"v-show",value:t.todos.length,expression:"todos.length"}],staticClass:"main"},[o("input",{staticClass:"toggle-all",attrs:{id:"toggle-all",type:"checkbox"},domProps:{checked:t.allChecked},on:{change:function(e){return t.toggleAll({done:!t.allChecked})}}}),t._v(" "),o("label",{attrs:{for:"toggle-all"}}),t._v(" "),o("ul",{staticClass:"todo-list"},t._l(t.filteredTodos,function(e,n){return o("todo",{key:n,attrs:{todo:e},on:{toggleTodo:t.toggleTodo,editTodo:t.editTodo,deleteTodo:t.deleteTodo}})}),1)]),t._v(" "),o("footer",{directives:[{name:"show",rawName:"v-show",value:t.todos.length,expression:"todos.length"}],staticClass:"footer"},[o("span",{staticClass:"todo-count"},[o("strong",[t._v(t._s(t.remaining))]),t._v("\n      "+t._s(t._f("pluralize")(t.remaining,"item"))+" left\n    ")]),t._v(" "),o("ul",{staticClass:"filters"},t._l(t.filters,function(e,n){return o("li",{key:n},[o("a",{class:{selected:t.visibility===n},on:{click:function(e){e.preventDefault(),t.visibility=n}}},[t._v(t._s(t._f("capitalize")(n)))])])}),0),t._v(" "),o("ul",{staticClass:"filters"},[o("a",[t._v("epaper IP :")]),t._v(" "),o("input",{staticClass:"set-ip",attrs:{id:"set-ip-id",placeholder:t.epaper.epaperIP},on:{keyup:function(e){return!e.type.indexOf("key")&&t._k(e.keyCode,"enter",13,e.key,"Enter")?null:t.setIP(e)}}}),t._v(" "),o("a",[t._v("rotate :")]),t._v(" "),o("input",{staticClass:"rotate",attrs:{placeholder:t.picRotate},on:{keyup:function(e){return!e.type.indexOf("key")&&t._k(e.keyCode,"enter",13,e.key,"Enter")?null:t.rotate(e)}}})])])])},staticRenderFns:[]};var v=o("VU/8")(h,g,!1,function(t){o("IpDB")},null,null).exports;n.a.use(r.a);var m=new r.a({routes:[{path:"/",name:"index",component:v}]});n.a.config.productionTip=!1,new n.a({el:"#app",router:m,components:{App:s},template:"<App/>"})},gsu9:function(t,e){}},["NHnr"]);
//# sourceMappingURL=app.c85aa05e6cb68614f714.js.map