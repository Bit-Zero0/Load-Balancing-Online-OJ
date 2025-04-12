  // 主题切换功能
  const themeToggle = document.getElementById('themeToggle');
  const icon = themeToggle.querySelector('i');
  const hljsLight = document.getElementById('hljs-light');
  const hljsDark = document.getElementById('hljs-dark');

  // 检查本地存储中的主题设置
  const currentTheme = localStorage.getItem('theme') || 'light';
  document.documentElement.setAttribute('data-theme', currentTheme);

  // 更新代码高亮样式
  if (currentTheme === 'dark') {
      hljsLight.disabled = true;
      hljsDark.disabled = false;
  } else {
      hljsLight.disabled = false;
      hljsDark.disabled = true;
  }

  // 更新图标
  updateIcon();

  themeToggle.addEventListener('click', () => {
      const currentTheme = document.documentElement.getAttribute('data-theme');
      const newTheme = currentTheme === 'light' ? 'dark' : 'light';

      document.documentElement.setAttribute('data-theme', newTheme);
      localStorage.setItem('theme', newTheme);

      // 切换代码高亮样式
      if (newTheme === 'dark') {
          hljsLight.disabled = true;
          hljsDark.disabled = false;
      } else {
          hljsLight.disabled = false;
          hljsDark.disabled = true;
      }

      // 更新编辑器主题
      if (newTheme === 'dark') {
          editor.setTheme("ace/theme/monokai");
      } else {
          editor.setTheme("ace/theme/chrome");
      }

      updateIcon();

      // 重新渲染Markdown内容以更新代码高亮
      renderMarkdown();
  });

  function updateIcon() {
      const currentTheme = document.documentElement.getAttribute('data-theme');
      if (currentTheme === 'dark') {
          icon.className = 'fas fa-moon';
      } else {
          icon.className = 'fas fa-sun';
      }
  }

  // 初始化ACE编辑器
  let editor;

  function initEditor() {
      editor = ace.edit("code");
      const currentTheme = document.documentElement.getAttribute('data-theme') || 'light';

      // 设置编辑器主题
      editor.setTheme(currentTheme === 'dark' ? "ace/theme/monokai" : "ace/theme/chrome");
      editor.session.setMode("ace/mode/c_cpp");
      editor.setFontSize(16);
      editor.getSession().setTabSize(4);
      editor.setReadOnly(false);

      // 启用自动完成
      ace.require("ace/ext/language_tools");
      editor.setOptions({
          enableBasicAutocompletion: true,
          enableSnippets: true,
          enableLiveAutocompletion: true
      });
  }

  // 监听主题变化
  function watchTheme() {
      const observer = new MutationObserver((mutations) => {
          mutations.forEach((mutation) => {
              if (mutation.attributeName === 'data-theme') {
                  const theme = document.documentElement.getAttribute('data-theme');
                  editor.setTheme(theme === 'dark' ? "ace/theme/monokai" : "ace/theme/chrome");
              }
          });
      });

      observer.observe(document.documentElement, {
          attributes: true,
          attributeFilter: ['data-theme']
      });
  }

  // 使用 marked.js 渲染 Markdown 内容
  function renderMarkdown() {
      // 配置 marked 选项
      marked.setOptions({
          highlight: function (code, lang) {
              if (lang && hljs.getLanguage(lang)) {
                  return hljs.highlight(code, { language: lang }).value;
              }
              return hljs.highlightAuto(code).value;
          },
          breaks: true,        // 允许换行符转换为 <br>
          gfm: true,           // 启用 GitHub 风格的 Markdown
          headerIds: true,     // 为标题添加 id
          mangle: false,       // 不转义内联 HTML
          sanitize: false,     // 不净化输出
          smartLists: true,    // 使用更智能的列表行为
          smartypants: true,   // 使用更智能的标点符号
          xhtml: false         // 不使用自闭合标签
      });

      // 获取原始 Markdown 内容
      const markdownContent = $("#markdown-desc");
      const rawMarkdown = markdownContent.text();

      // 将 Markdown 转换为 HTML 并插入页面
      markdownContent.html(marked.parse(rawMarkdown));

      // 初始化代码高亮
      $('pre code').each(function (i, block) {
          hljs.highlightBlock(block);
      });
  }

  $(document).ready(function () {
      initEditor();
      watchTheme();
      initResizer();
      renderMarkdown();
  });

  // 左右面板拖拽调整
  const resizer = document.getElementById('dragMe');
  const leftPanel = document.querySelector('.left-desc');
  const rightPanel = document.querySelector('.right-code');
  const container = document.querySelector('.code-container');

  let isResizing = false;
  let startX = 0;
  let startLeftWidth = 0;

  function initResizer() {
      if (window.innerWidth <= 768) {
          resizer.style.display = 'none';
          return;
      }

      resizer.style.display = 'block';
      const containerRect = container.getBoundingClientRect();
      const leftRect = leftPanel.getBoundingClientRect();
      const leftPercentage = (leftRect.width / containerRect.width) * 100;
      resizer.style.left = `${leftPercentage}%`;
  }

  window.addEventListener('load', initResizer);
  window.addEventListener('resize', initResizer);

  resizer.addEventListener('mousedown', (e) => {
      if (window.innerWidth <= 768) return;

      isResizing = true;
      startX = e.clientX;
      startLeftWidth = leftPanel.getBoundingClientRect().width;
      
      document.body.classList.add('resizing');
      document.addEventListener('mousemove', handleMouseMove);
      document.addEventListener('mouseup', handleMouseUp);
  });

  function handleMouseMove(e) {
      if (!isResizing || window.innerWidth <= 768) return;

      requestAnimationFrame(() => {
          const containerRect = container.getBoundingClientRect();
          const deltaX = e.clientX - startX;
          const newLeftWidth = startLeftWidth + deltaX;
          const containerWidth = containerRect.width;
          
          let leftPercentage = (newLeftWidth / containerWidth) * 100;
          leftPercentage = Math.max(20, Math.min(80, leftPercentage));
          const rightPercentage = 100 - leftPercentage;

          leftPanel.style.flex = `0 0 ${leftPercentage}%`;
          rightPanel.style.flex = `0 0 ${rightPercentage}%`;
          resizer.style.left = `${leftPercentage}%`;
      });
  }

  function handleMouseUp() {
      isResizing = false;
      document.body.classList.remove('resizing');
      document.removeEventListener('mousemove', handleMouseMove);
      document.removeEventListener('mouseup', handleMouseUp);
  }

  const submitSection = document.getElementById('submitSection');
  const submitResizer = document.getElementById('submitDragMe');
  const showSubmitBtn = document.getElementById('showSubmitBtn');

  let isSubmitResizing = false;
  let initialSubmitHeight = 0;
  let startY = 0;
  const minHeight = 50; // 最小高度，低于此高度时隐藏

  function initSubmitResizer() {
      initialSubmitHeight = submitSection.offsetHeight;
  }

  window.addEventListener('load', initSubmitResizer);
  window.addEventListener('resize', initSubmitResizer);

  submitResizer.addEventListener('mousedown', (e) => {
      isSubmitResizing = true;
      startY = e.clientY;
      initialSubmitHeight = submitSection.offsetHeight;
      
      submitSection.classList.add('resizing');
      document.body.classList.add('resizing');
      document.addEventListener('mousemove', handleSubmitMouseMove);
      document.addEventListener('mouseup', handleSubmitMouseUp);
  });

  function handleSubmitMouseMove(e) {
      if (!isSubmitResizing) return;
      
      requestAnimationFrame(() => {
          const deltaY = e.clientY - startY;
          const newHeight = initialSubmitHeight - deltaY;

          if (newHeight < minHeight) {
              hideSubmitSection();
          } else {
              submitSection.style.display = 'flex';
              submitSection.style.height = `${newHeight}px`;
              showSubmitBtn.classList.add('hidden');
          }
      });
  }

  function handleSubmitMouseUp() {
      isSubmitResizing = false;
      submitSection.classList.remove('resizing');
      document.body.classList.remove('resizing');
      document.removeEventListener('mousemove', handleSubmitMouseMove);
      document.removeEventListener('mouseup', handleSubmitMouseUp);

      if (submitSection.offsetHeight < minHeight) {
          hideSubmitSection();
      } else {
          initialSubmitHeight = submitSection.offsetHeight;
      }
  }

  function hideSubmitSection() {
      submitSection.style.display = 'none';
      showSubmitBtn.classList.remove('hidden');
  }

  function showSubmitSection() {
      submitSection.style.display = 'flex';
      submitSection.style.height = `${initialSubmitHeight}px`;
      showSubmitBtn.classList.add('hidden');
  }

  // 初始化时设置正确的显示状态
  window.addEventListener('load', () => {
      if (submitSection.style.display === 'none') {
          showSubmitBtn.classList.remove('hidden');
      } else {
          showSubmitBtn.classList.add('hidden');
      }
      renderMarkdown();
  });

  // 代码提交功能
  function submit() {
      const code = editor.getSession().getValue();
      const number = $("#number").text();
      const judge_url = "/judge/" + number;
      const result_div = $(".result");
      
      result_div.html("<p>正在评测中...</p>");

      $.ajax({
          method: 'Post',
          url: judge_url,
          dataType: 'json',
          contentType: 'application/json;charset=utf-8',
          data: JSON.stringify({
              'code': code,
              'input': ''
          }),
          success: function (data) {
              show_result(data);
          },
          error: function () {
              result_div.html("<p style='color:red'>评测请求失败，请稍后重试</p>");
          }
      });
  }

  function show_result(data) {
      const result_div = $(".result");
      result_div.empty();

      const _status = data.status;
      const _reason = data.reason;

      const reason_lable = $("<p>", {
          text: _reason
      });
      reason_lable.appendTo(result_div);

      if (_status == 0) {
          const _stdout = data.stdout;
          const _stderr = data.stderr;

          if (_stdout) {
              const stdout_lable = $("<pre>", {
                  text: _stdout
              });
              stdout_lable.appendTo(result_div);
          }

          if (_stderr) {
              const stderr_lable = $("<pre>", {
                  text: _stderr,
                  style: "color: #cc0000;"
              });
              stderr_lable.appendTo(result_div);
          }
      }
  }
