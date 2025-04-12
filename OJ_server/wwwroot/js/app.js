// 主题切换功能
function initTheme() {
    const themeToggle = document.getElementById('themeToggle');
    if (!themeToggle) return;

    // 从localStorage获取主题设置
    const savedTheme = localStorage.getItem('theme') || 'light';
    document.documentElement.setAttribute('data-theme', savedTheme);
    setTheme(savedTheme);

    // 切换主题事件
    themeToggle.addEventListener('click', () => {
        const currentTheme = document.documentElement.getAttribute('data-theme');
        const newTheme = currentTheme === 'light' ? 'dark' : 'light';
        document.documentElement.setAttribute('data-theme', newTheme);
        setTheme(newTheme);
    });
}

// 设置主题
function setTheme(theme) {
    // 保存主题设置
    localStorage.setItem('theme', theme);
    
    // 更新主题图标
    const icon = document.querySelector('#themeToggle i');
    if (icon) {
        icon.className = theme === 'dark' ? 'fas fa-moon' : 'fas fa-sun';
    }

    // 更新代码编辑器主题（如果存在）
    try {
        const editor = ace && ace.edit && ace.edit("code");
        if (editor) {
            editor.setTheme(theme === 'dark' ? "ace/theme/monokai" : "ace/theme/chrome");
        }
    } catch (e) {
        console.log('ACE editor not found');
    }

    // 更新代码高亮样式
    const hljsLight = document.getElementById('hljs-light');
    const hljsDark = document.getElementById('hljs-dark');
    if (hljsLight && hljsDark) {
        hljsLight.disabled = theme === 'dark';
        hljsDark.disabled = theme === 'light';

        // 如果页面上有代码块，重新应用高亮
        if (window.hljs) {
            document.querySelectorAll('pre code').forEach(block => {
                hljs.highlightBlock(block);
            });
        }
    }
}

// 确保在DOM加载完成后初始化主题
document.addEventListener('DOMContentLoaded', initTheme);
