/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*
* @file ExprHighlighter.h
* @brief A Qt syntax highlighter for the SeExpr language
* @author  aselle
*/
#ifndef _ExprHighlighter_h_
#define _ExprHighlighter_h_
#include <QSyntaxHighlighter>
#include <QPalette>
#include <iostream>

class ExprHighlighter : public QSyntaxHighlighter {
    struct HighlightingRule {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat variableFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat operatorFormat;

    int lightness;

  public:
    ExprHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent), lightness(130) { init(); }

    ExprHighlighter(QTextEdit* edit) : QSyntaxHighlighter(edit), lightness(130) { init(); }

    void fixStyle(const QPalette& palette) {
        lightness = palette.color(QPalette::Base).value() < 127 ? 250 : 130;
        init();
    }

    void init() {
        HighlightingRule rule;
        highlightingRules.clear();

        // Operator highlighting, disabled for now
        // operatorFormat.setForeground(QColor::fromHsv(50,128,lightness));
        // QStringList operatorPatterns;
        // operatorPatterns<<"(?:->)|(?:[()\\+-/\\*%\\^:\\?\\[\\]])";
        // foreach (QString pattern,operatorPatterns){
        //    rule.pattern=QRegExp(pattern);
        //    rule.format=operatorFormat;
        //    highlightingRules.append(rule);
        //}

        numberFormat.setForeground(QColor::fromHsv(180, 204, lightness));
        rule.pattern = QRegExp("\\b[0-9]*\\.[0-9]*)?|[0-9]+\\b");  // \\b?[^\\$][A-Za-z][A-Za-z0-9]*\\b");
        rule.format = numberFormat;
        // highlightingRules.append(rule);

        variableFormat.setForeground(QColor::fromHsv(200, 153, lightness));
        // variableFormat.setFontWeight(QFont::Bold);
        rule.pattern = QRegExp("\\$[A-Za-z][A-Za-z0-9]*\\b");
        rule.format = variableFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(QColor::fromHsv(210, 128, lightness));
        rule.pattern = QRegExp("#[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);
    }

    void highlightBlock(const QString& text) {
        foreach(HighlightingRule rule, highlightingRules) {
            QRegExp expression(rule.pattern);
            int index = text.indexOf(expression);
            while (index >= 0) {
                int length = expression.matchedLength();
                setFormat(index, length, rule.format);
                index = text.indexOf(expression, index + length);
            }
        }
        setCurrentBlockState(0);
    }
};
#endif
