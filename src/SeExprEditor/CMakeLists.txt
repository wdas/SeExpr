# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND BASED ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

find_package(Qt4 COMPONENTS QtCore QtGui QtOpenGL) # find and setup Qt4 for this project
if(QT4_FOUND)

    BuildParserScanner(SeExprSpecParserLex SeExprSpecParser SeExprSpec editor_parser_cpp)
    set(SeExprEditor_MOC_HDRS QdSeBrowser.h QdSeCCurve.h QdSeControlCollection.h
      QdSeControl.h QdSeCurve.h QdSeDialog.h QdSeEditor.h QdSeFileDialog.h QdSeGrapher2d.h
      QdSePopupDocumentation.h QdSeShortEdit.h)
    set(SeExprEditor_CPPS QdSeFileDialog.cpp QdSeControl.cpp QdSeEditor.cpp dExprEditor.cpp
      QdSeShortEdit.cpp QdSeCurve.cpp  QdSeCCurve.cpp QdSeEditableExpression.cpp 
      QdSePopupDocumentation.cpp QdSeCompletionModel.cpp QdSeDialog.cpp QdSeControlCollection.cpp
      QdSeGrapher2d.cpp QdSeBrowser.cpp QdSeExpr.cpp)

    qt4_wrap_cpp(SeExprEditor_MOC_SRCS ${SeExprEditor_MOC_HDRS})
    
    add_library(SeExprEditor SHARED ${SeExprEditor_CPPS} ${SeExprEditor_MOC_SRCS} ${editor_parser_cpp})
    
    install(TARGETS SeExprEditor DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${SeExprEditor_MOC_HDRS} DESTINATION include/SeExprEditor)
    include_directories(${QT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(SeExprEditor ${QT_QTCORE_LIBRARY})
    target_link_libraries(SeExprEditor ${QT_QTGUI_LIBRARY})
    target_link_libraries(SeExprEditor ${QT_QTOPENGL_LIBRARY})
    target_link_libraries(SeExprEditor ${SEEXPR_LIBRARIES})
endif(QT4_FOUND)
