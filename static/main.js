var Main = {
  data() {
    return {
      example : [],
      history : [],
      content : '#include <iostream>\n\nint main(){\n  std::cout << "Hello world!" << std::endl;\n}\n',
    }
  },
  mounted: function () {
    // attach codemirror to dom id
    this.code_editor = CodeMirror.fromTextArea(document.getElementById("code"), {
      mode: "text/x-c++src",
      lineNumbers: true,  // display line numbers
      lineWrapping: true, // enable code wrapping
      foldGutter: true,
      gutters: ["CodeMirror-linenumbers", "CodeMirror-foldgutter"],
      matchBrackets: true,  // enable brackets matching
    });
    this.code_editor.setSize(null, 500);     // set code block width
    this.code_editor.setValue(this.content);
    // console block
    this.console_editor = CodeMirror.fromTextArea(document.getElementById("console"), {
      readOnly: true,       // read only
    });
    this.console_editor.setSize(null, 500);
    // load example lists
    this.GetExample();
    // load history
    this.LoadHistory();
  },
  methods: {
    GetExample : function() {
      var that = this;
      var url = "/example_list";
      $.ajax({
        type: "POST",
        contentType: "application/json",
        url: url,
        success: function (res) {
          var example = [];
          for (var i = 0; i < res.length; ++i) {
            example.push(res[i]);
          }
          that.example = example;
        }
      });
    },
    Update : function (name) {
      var that = this;
      var url = "/example";
      $.ajax({
        type: "POST",
        contentType: "application/json",
        url: url,
        data : JSON.stringify({'name' : name}),
        success: function (res) {
          that.code_editor.setValue(res['content']);
        }
      });
    },
    Clear : function () {
      this.console_editor.setValue("");
    },
    LoadHistory : function () {
      var history_store = localStorage.getItem("neonhistory");
      if (history_store == null) {
        return;
      }
      var history_array = history_store.split(",");
      var history_tmp = [];
      for (var i = 0; i < history_array.length; ++i) {
        var his_date = new Date();
        his_date.setTime(parseInt(history_array[i]));
        history_tmp.push({'name' : history_array[i], 'label' : his_date.toLocaleString()})
      }
      this.history = history_tmp;
    },
    Save : function () {
      var timestamp = new Date().getTime();
      var content = this.code_editor.getValue();
      var history_store = localStorage.getItem("neonhistory");
      if (history_store == null) {
        history_store = "";
      }
      var history_array = history_store.split(",");
      if (localStorage.getItem(history_array[0]) == content) {
        return;
      }
      localStorage.setItem(timestamp, content);
      if (history_array.length >= 10) {
        var inx = history_store.lastIndexOf(",");
        var tmp = history_store.substring(0, inx);
        tmp = timestamp + "," + tmp;
        localStorage.setItem("neonhistory", tmp);
        localStorage.removeItem(history_array[history_array.length - 1]);
      } else {
        if (history_array.length == 1 && history_store == "") {
          localStorage.setItem("neonhistory", timestamp);
        } else {
          localStorage.setItem("neonhistory", timestamp + "," + history_store);
        }
      }
      this.LoadHistory();
    },
    Reload : function (name) {
      this.code_editor.setValue(localStorage.getItem(name));
    },
    Run : function () {
      this.Clear();
      this.Save();
      var content = this.code_editor.getValue();
      var that = this;
      var url = "/run";
      $.ajax({
        type: "POST",
        contentType: "application/json",
        url: url,
        data: JSON.stringify({'content' : content}),
        success: function (res) {
          that.console_editor.setValue(res['content']); 
        }
      });
    },
  }
};
var Ctor = Vue.extend(Main);
new Ctor().$mount('#app');

