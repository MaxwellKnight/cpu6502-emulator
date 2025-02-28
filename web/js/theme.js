document.addEventListener('DOMContentLoaded', function() {
	const themeToggle = document.getElementById('themeToggle');
	const htmlElement = document.documentElement;

	// Check for saved theme preference
	const savedTheme = localStorage.getItem('preferred-theme');
	if (savedTheme) {
		htmlElement.setAttribute('data-bs-theme', savedTheme);
		updateThemeToggle(savedTheme);
	}

	// Theme toggle handler
	themeToggle.addEventListener('click', function() {
		const currentTheme = htmlElement.getAttribute('data-bs-theme');
		const newTheme = currentTheme === 'dark' ? 'light' : 'dark';

		htmlElement.setAttribute('data-bs-theme', newTheme);
		updateThemeToggle(newTheme);

		// Save preference
		localStorage.setItem('preferred-theme', newTheme);
	});

	function updateThemeToggle(theme) {
		const themeToggle = document.getElementById('themeToggle');
		if (theme === 'light') {
			themeToggle.innerHTML = '<i class="bi bi-sun-fill me-2"></i><span>Light Mode</span>';
		} else {
			themeToggle.innerHTML = '<i class="bi bi-moon-stars-fill me-2"></i><span>Dark Mode</span>';
		}
	}
});
