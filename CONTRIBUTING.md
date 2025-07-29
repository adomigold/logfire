# Contributing to Logfire 🔥

First off, thank you for considering contributing to Logfire! We appreciate your time and effort in making this project better.

---

## Code of Conduct

Please be respectful and constructive. We aim to foster a welcoming community.

---

## Coding Standards

- **Language:** C (C99 standard preferred)  
- **Formatting:**  
  - Use 4 spaces per indentation (no tabs)  
  - Keep line length <= 80 characters where possible  
  - Use clear, descriptive variable and function names  
- **Comments:**  
  - Comment your code, especially public APIs and complex logic  
  - Use consistent style for comments (`//` for inline, `/* ... */` for blocks)  
- **File Headers:**  
  - Add SPDX license headers to all new `.c` and `.h` files (see existing files for examples)  
- **Error Handling:**  
  - Handle all errors gracefully, returning error codes or printing meaningful messages  
- **Memory Management:**  
  - Avoid memory leaks and dangling pointers; free allocated memory properly

---

## Reporting Issues

To help us understand and resolve issues quickly, please:

- Search existing issues to avoid duplicates  
- Provide a clear and descriptive title  
- Include steps to reproduce the issue  
- Share your environment details (OS, compiler version, Logfire version)  
- Provide logs, error messages, or screenshots if applicable  

Report issues on the [GitHub Issues page](https://github.com/adomigold/logfire/issues).

---

## Requesting Features

We welcome ideas and improvements! When requesting a feature:

- Explain the problem it solves or the benefit it brings  
- Provide examples or use cases if possible  
- Suggest a possible implementation approach if you have one  

Submit feature requests as GitHub issues labeled `enhancement`.

---

## Adding New Log Format Support

Logfire currently supports Apache and Nginx access logs. To add support for a new log format:

1. **Fork the repository and create a new branch.**

2. **Implement a parser for the new log format:**

   - Add a new function in `parser.c` (e.g., `parseMyFormatLogLine`) that takes a raw log line and extracts fields into the `LogEntry` structure.
   - Handle parsing errors gracefully.
   - Add tests for your parser if possible.

3. **Integrate the new parser:**

   - Modify `parseLogFile` in `parser.c` or create a dispatch mechanism to detect and delegate parsing based on the log format or user input.
   - Update CLI options if needed to allow users to specify the log format.

4. **Update documentation:**

   - Add details about the new format to `README.md` under supported log types.
   - Update examples if applicable.

5. **Test thoroughly:**

   - Run existing tests to ensure no regressions.
   - Add new tests covering edge cases for the new format.

6. **Submit a pull request with a clear description of your changes.**

---

## Getting Help

If you have questions or want to discuss ideas, please open a GitHub issue.

---

Thanks again for helping improve Logfire! Together, we make logs easier to understand.

---
