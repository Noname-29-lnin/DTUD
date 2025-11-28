import pandas as pd

def escape_latex(s):
    """
    Escape ký tự đặc biệt để tránh lỗi khi đưa vào LaTeX.
    """
    if not isinstance(s, str):
        return s
    replacements = {
        '\\': r'\textbackslash{}',
        '_': r'\_',
        '%': r'\%',
        '&': r'\&',
        '#': r'\#',
        '{': r'\{',
        '}': r'\}',
        '$': r'\$',
        '^': r'\^{}',
        '~': r'\~{}',
    }
    for key, value in replacements.items():
        s = s.replace(key, value)
    return s


def excel_to_latex(
    excel_file,
    sheet_name=0,
    output_tex="table_output.tex",
    longtable=False
):
    """
    Chuyển bảng Excel thành bảng LaTeX.

    excel_file  : đường dẫn file Excel
    sheet_name  : tên sheet hoặc index sheet
    output_tex  : file .tex sẽ xuất ra
    longtable   : True = dùng longtable, False = dùng tabular
    """

    df = pd.read_excel(excel_file, sheet_name=sheet_name)

    # Escape latex
    df = df.applymap(escape_latex)

    columns = list(df.columns)

    col_format = "|".join(["c"] * len(columns))
    col_format = "|" + col_format + "|"

    lines = []

    if longtable:
        # ------------------ LONGTABLE ------------------
        lines.append("\\begin{longtable}{" + col_format + "}\n")
        lines.append("\\hline\n")
        lines.append(" & ".join(columns) + " \\\\\n")
        lines.append("\\hline\n")
        lines.append("\\endfirsthead\n")

        # Header ở trang sau
        lines.append("\\multicolumn{" + str(len(columns)) +
                     "}{c}{\\tablename\\ \\thetable{} -- continued}\\\\\n")
        lines.append("\\hline\n")
        lines.append(" & ".join(columns) + " \\\\\n")
        lines.append("\\hline\n")
        lines.append("\\endhead\n")

        # Footer
        lines.append("\\hline\n")
        lines.append("\\multicolumn{" + str(len(columns)) +
                     "}{r}{Continued on next page} \\\\\n")
        lines.append("\\endfoot\n")

        lines.append("\\hline\n")
        lines.append("\\endlastfoot\n")

        # Data rows
        for _, row in df.iterrows():
            row_str = " & ".join(str(v) for v in row.values) + " \\\\\n"
            lines.append(row_str)

        lines.append("\\end{longtable}\n")

    else:
        # ------------------ TABULAR ------------------
        lines.append("\\begin{table}[h]\n\\centering\n")
        lines.append("\\begin{tabular}{" + col_format + "}\n")
        lines.append("\\hline\n")
        lines.append(" & ".join(columns) + " \\\\\n")
        lines.append("\\hline\n")

        for _, row in df.iterrows():
            row_str = " & ".join(str(v) for v in row.values) + " \\\\\n"
            lines.append(row_str)

        lines.append("\\hline\n")
        lines.append("\\end{tabular}\n")
        lines.append("\\end{table}\n")

    # Ghi file
    with open(output_tex, "w", encoding="utf-8") as f:
        f.writelines(lines)

    print(f"[OK] LaTeX table saved to: {output_tex}")


# ======================
# Ví dụ sử dụng
# ======================
if __name__ == "__main__":
    excel_to_latex(
        excel_file="table.xlsx",
        sheet_name=0,
        output_tex="adc.tex",
        longtable=True
    )
